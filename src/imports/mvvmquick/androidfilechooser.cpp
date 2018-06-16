#include "androidfilechooser.h"
#include <QtAndroidExtras/QtAndroid>
#include <QtAndroidExtras/QAndroidJniEnvironment>
using namespace QtMvvm;

AndroidFileChooser::AndroidFileChooser(QObject *parent) :
	QObject(parent),
	_title(),
	_folderUrl(),
	_type(OpenDocument),
	_mimeTypes(QStringLiteral("*/*")),
	_flags(OpenableFlag | AlwaysGrantWriteFlag),
	_active(false),
	_result()
{}

AndroidFileChooser::~AndroidFileChooser() = default;

AndroidFileChooser::ChooserFlags AndroidFileChooser::chooserFlags() const
{
	return _flags;
}

QVariant AndroidFileChooser::result() const
{
	return _result;
}

void AndroidFileChooser::open()
{
	if(_active)
		return;

	QAndroidJniObject intent;
	switch (_type) {
	case GetContent:
		intent = createGetIntent();
		break;
	case OpenDocument:
		intent = createOpenIntent();
		break;
	case OpenMultipleDocuments:
		intent = createOpenMultipleIntent();
		break;
	case CreateDocument:
		intent = createSaveIntent();
		break;
	case OpenDocumentTree:
		intent = createOpenTreeIntent();
		break;
	default:
		Q_UNREACHABLE();
		break;
	}

	auto chooserIntent = QAndroidJniObject::callStaticObjectMethod("android/content/Intent", "createChooser",
																   "(Landroid/content/Intent;Ljava/lang/CharSequence;)Landroid/content/Intent;",
																   intent.object(),
																   QAndroidJniObject::fromString(_title).object());
	_active = true;
	QtAndroid::startActivity(chooserIntent,
							 _flags.testFlag(PersistPermissionsFlag) ? RequestCodePersist : RequestCodeNormal,
							 this);
}

void AndroidFileChooser::setChooserFlags(ChooserFlags chooserFlags)
{
	if (_flags == chooserFlags)
		return;

	_flags = chooserFlags;
	emit chooserFlagsChanged(chooserFlags);
}

void AndroidFileChooser::handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data)
{
	if(receiverRequestCode == RequestCodeNormal || receiverRequestCode == RequestCodePersist) {
		static const auto RESULT_OK = QAndroidJniObject::getStaticField<jint>("android/app/Activity", "RESULT_OK");
		static const auto FLAG_GRANT_READ_URI_PERMISSION = QAndroidJniObject::getStaticField<jint>("android/content/Intent", "FLAG_GRANT_READ_URI_PERMISSION");
		static const auto FLAG_GRANT_WRITE_URI_PERMISSION = QAndroidJniObject::getStaticField<jint>("android/content/Intent", "FLAG_GRANT_WRITE_URI_PERMISSION");

		if(resultCode == RESULT_OK) {
			if(receiverRequestCode == RequestCodePersist) {
				auto flags = data.callMethod<jint>("getFlags");
				flags &= (FLAG_GRANT_READ_URI_PERMISSION | FLAG_GRANT_WRITE_URI_PERMISSION);

				auto resolver = QtAndroid::androidContext().callObjectMethod("getContentResolver", "()Landroid/content/ContentResolver;");
				resolver.callMethod<void>("takePersistableUriPermission", "(Landroid/net/Uri;I)V",
										  data.callObjectMethod("getData", "()Landroid/net/Uri;").object(),
										  flags);
			}

			_result.clear();
			if(_type == OpenMultipleDocuments) {
				auto clipData = data.callObjectMethod("getClipData", "()Landroid/content/ClipData;");
				if(clipData.isValid()) {
					QList<QUrl> urls;
					const auto cnt = clipData.callMethod<jint>("getItemCount");
					for(auto i = 0; i < cnt; i++) {
						auto item = clipData.callObjectMethod("getItemAt", "(I)Landroid/content/ClipData$Item;", i);
						urls.append(item.callObjectMethod("getUri", "()Landroid/net/Uri;").toString());
					}
					_result = QVariant::fromValue(urls);
				}
			}

			if(!_result.isValid())
				_result = QUrl(data.callObjectMethod("getDataString", "()Ljava/lang/String;").toString());
			emit resultChanged(_result);
			_active = false;
			emit accepted();
		} else {
			_active = false;
			emit rejected();
		}
	}
}

QAndroidJniObject AndroidFileChooser::createGetIntent()
{
	static const auto ACTION_GET_CONTENT = QAndroidJniObject::getStaticObjectField<jstring>("android/content/Intent", "ACTION_GET_CONTENT");
	static const auto EXTRA_LOCAL_ONLY = QAndroidJniObject::getStaticObjectField<jstring>("android/content/Intent", "EXTRA_LOCAL_ONLY");

	QAndroidJniObject intent("android/content/Intent", "(Ljava/lang/String;)V",
							 ACTION_GET_CONTENT.object());
	setupBasic(intent);

	if(_flags.testFlag(LocalOnlyFlag)) {
		intent.callObjectMethod("putExtra", "(Ljava/lang/String;Z)Landroid/content/Intent;",
								EXTRA_LOCAL_ONLY.object(), true);
	}

	return intent;
}

QAndroidJniObject AndroidFileChooser::createOpenIntent()
{
	static const auto ACTION_OPEN_DOCUMENT = QAndroidJniObject::getStaticObjectField<jstring>("android/content/Intent", "ACTION_OPEN_DOCUMENT");

	QAndroidJniObject intent("android/content/Intent", "(Ljava/lang/String;)V",
							 ACTION_OPEN_DOCUMENT.object());
	setupBasic(intent);

	return intent;
}

QAndroidJniObject AndroidFileChooser::createOpenMultipleIntent()
{
	static const auto EXTRA_ALLOW_MULTIPLE = QAndroidJniObject::getStaticObjectField<jstring>("android/content/Intent", "EXTRA_ALLOW_MULTIPLE");

	auto intent = createOpenIntent();
	intent.callObjectMethod("putExtra", "(Ljava/lang/String;Z)Landroid/content/Intent;",
							EXTRA_ALLOW_MULTIPLE.object(), true);

	return intent;
}

QAndroidJniObject AndroidFileChooser::createSaveIntent()
{
	static const auto ACTION_CREATE_DOCUMENT = QAndroidJniObject::getStaticObjectField<jstring>("android/content/Intent", "ACTION_CREATE_DOCUMENT");

	QAndroidJniObject intent("android/content/Intent", "(Ljava/lang/String;)V",
							 ACTION_CREATE_DOCUMENT.object());
	setupBasic(intent);

	return intent;
}

QAndroidJniObject AndroidFileChooser::createOpenTreeIntent()
{
	static const auto ACTION_OPEN_DOCUMENT_TREE = QAndroidJniObject::getStaticObjectField<jstring>("android/content/Intent", "ACTION_OPEN_DOCUMENT_TREE");

	QAndroidJniObject intent("android/content/Intent", "(Ljava/lang/String;)V",
							 ACTION_OPEN_DOCUMENT_TREE.object());
	setupBasic(intent, true);

	return intent;
}

void AndroidFileChooser::setupBasic(QAndroidJniObject &intent, bool asTree)
{
	static const auto CATEGORY_OPENABLE = QAndroidJniObject::getStaticObjectField<jstring>("android/content/Intent", "CATEGORY_OPENABLE");

	static const auto FLAG_GRANT_PERSISTABLE_URI_PERMISSION = QAndroidJniObject::getStaticField<jint>("android/content/Intent", "FLAG_GRANT_PERSISTABLE_URI_PERMISSION");
	static const auto FLAG_GRANT_READ_URI_PERMISSION = QAndroidJniObject::getStaticField<jint>("android/content/Intent", "FLAG_GRANT_READ_URI_PERMISSION");
	static const auto FLAG_GRANT_WRITE_URI_PERMISSION = QAndroidJniObject::getStaticField<jint>("android/content/Intent", "FLAG_GRANT_WRITE_URI_PERMISSION");

	static const auto EXTRA_MIME_TYPES = QAndroidJniObject::getStaticObjectField<jstring>("android/content/Intent", "EXTRA_MIME_TYPES");
	static const auto EXTRA_INITIAL_URI = [](){
		if(QtAndroid::androidSdkVersion() >= 26) //Android Oreo
			return QAndroidJniObject::getStaticObjectField<jstring>("android/provider/DocumentsContract", "EXTRA_INITIAL_URI");
		else
			return QAndroidJniObject();
	}();

	//set the acceptable mimetypes
	if(!asTree) {
		if(_mimeTypes.size() == 1) {
			intent.callObjectMethod("setTypeAndNormalize", "(Ljava/lang/String;)Landroid/content/Intent;",
									QAndroidJniObject::fromString(_mimeTypes.first()).object());
		} else {
			intent.callObjectMethod("setType", "(Ljava/lang/String;)Landroid/content/Intent;",
									QAndroidJniObject::fromString(QStringLiteral("*/*")).object());

			if(!_mimeTypes.isEmpty()) {
				QAndroidJniEnvironment env;
				auto strClass = env->FindClass("java/lang/String");
				auto strArray = QAndroidJniObject::fromLocalRef(env->NewObjectArray(_mimeTypes.size(), strClass, nullptr));
				for(auto i = 0; i < _mimeTypes.size(); i++) {
					auto mimeStr = QAndroidJniObject::callStaticObjectMethod("android/content/Intent", "normalizeMimeType",
																			 "(Ljava/lang/String;)Ljava/lang/String;",
																			 QAndroidJniObject::fromString(_mimeTypes[i]).object());
					env->SetObjectArrayElement(strArray.object<jobjectArray>(), i, mimeStr.object());
				}

				intent.callObjectMethod("putExtra", "(Ljava/lang/String;[Ljava/lang/String;)Landroid/content/Intent;",
										EXTRA_MIME_TYPES.object(), strArray.object());
			}
		}
	}

	//Set the intent flags
	auto aFlags = FLAG_GRANT_READ_URI_PERMISSION;
	if(_type != GetContent)
		aFlags |= FLAG_GRANT_PERSISTABLE_URI_PERMISSION;
	if(_type == CreateDocument || _flags.testFlag(AlwaysGrantWriteFlag))
		aFlags |= FLAG_GRANT_WRITE_URI_PERMISSION;
	intent.callObjectMethod("addFlags", "(I)Landroid/content/Intent;",
							aFlags);

	//set openable
	if(_flags.testFlag(OpenableFlag) && !asTree) {
		intent.callObjectMethod("addCategory", "(Ljava/lang/String;)Landroid/content/Intent;",
								CATEGORY_OPENABLE.object());
	}


	if(EXTRA_INITIAL_URI.isValid()) {
		auto uri = QAndroidJniObject::callStaticObjectMethod("android/net/Uri", "parse",
															 "(Ljava/lang/String;)Landroid/net/Uri;",
															 QAndroidJniObject::fromString(_folderUrl.toString()).object());
		intent.callObjectMethod("putExtra", "(Ljava/lang/String;Landroid/os/Parcelable;)Landroid/content/Intent;",
								EXTRA_INITIAL_URI.object(), uri.object());
	}
}

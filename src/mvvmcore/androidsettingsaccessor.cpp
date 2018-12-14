#include "androidsettingsaccessor.h"
#include "androidsettingsaccessor_p.h"
#include <QtCore/QDataStream>
#include <QtCore/QSet>
#include <QtAndroidExtras/QtAndroid>
#include <QtAndroidExtras/QAndroidJniExceptionCleaner>
#include "qtmvvm_logging_p.h"
using namespace QtMvvm;

AndroidSettingsAccessor::AndroidSettingsAccessor(QObject *parent) :
	ISettingsAccessor(parent),
	d{new AndroidSettingsAccessorPrivate{this}}
{}

AndroidSettingsAccessor::AndroidSettingsAccessor(const QString &file, QObject *parent) :
	AndroidSettingsAccessor{file, Private, parent}
{}

AndroidSettingsAccessor::AndroidSettingsAccessor(const QString &file, Mode mode, QObject *parent) :
	ISettingsAccessor(parent),
	d{new AndroidSettingsAccessorPrivate{this, file, mode}}
{}

AndroidSettingsAccessor::~AndroidSettingsAccessor()
{
	if(d->settings.isValid()) {
		QAndroidJniExceptionCleaner cleaner{QAndroidJniExceptionCleaner::OutputMode::Verbose};
		d->settings.callMethod<void>("unref");
	}
}

bool AndroidSettingsAccessor::contains(const QString &key) const
{
	if(!d->settings.isValid())
		return false;

	QAndroidJniExceptionCleaner cleaner{QAndroidJniExceptionCleaner::OutputMode::Verbose};
	return d->settings.callMethod<jboolean>("contains",
											"(Ljava/lang/String;)Z",
											QAndroidJniObject::fromString(key).object());
}

QVariant AndroidSettingsAccessor::load(const QString &key, const QVariant &defaultValue) const
{
	if(!d->settings.isValid())
		return defaultValue;

	QAndroidJniExceptionCleaner cleaner{QAndroidJniExceptionCleaner::OutputMode::Verbose};
	auto value = d->settings.callObjectMethod("load", "(Ljava/lang/String;)Ljava/lang/Object;",
											  QAndroidJniObject::fromString(key).object());
	if(value.isValid())
		return d->convertFromJava(value);
	else
		return defaultValue;
}

void AndroidSettingsAccessor::save(const QString &key, const QVariant &value)
{
	if(!d->settings.isValid())
		return;

	bool needSync = false;
	QAndroidJniExceptionCleaner cleaner{QAndroidJniExceptionCleaner::OutputMode::Verbose};
	QVariant strVar;
	switch(value.userType()) {
	case QMetaType::Bool:
		needSync = d->settings.callMethod<jboolean>("save", "(Ljava/lang/String;Z)Z",
													QAndroidJniObject::fromString(key).object(),
													static_cast<jboolean>(value.toBool()));
		break;
	case QMetaType::Int:
		needSync = d->settings.callMethod<jboolean>("save", "(Ljava/lang/String;I)Z",
													QAndroidJniObject::fromString(key).object(),
													static_cast<jint>(value.toInt()));
		break;
	case QMetaType::LongLong:
		needSync = d->settings.callMethod<jboolean>("save", "(Ljava/lang/String;J)Z",
													QAndroidJniObject::fromString(key).object(),
													static_cast<jlong>(value.toLongLong()));
		break;
	case QMetaType::Float:
		needSync = d->settings.callMethod<jboolean>("save", "(Ljava/lang/String;F)Z",
													QAndroidJniObject::fromString(key).object(),
													static_cast<jfloat>(value.toFloat()));
		break;
	default:
		if(value.userType() == qMetaTypeId<QSet<QString>>()) {
			auto set = value.value<QSet<QString>>();
			QAndroidJniObject hashSet{"java/util/HashSet", "(I)V", static_cast<jint>(set.size())};
			for(const auto &item : set) {
				hashSet.callMethod<jboolean>("add", "(Ljava/lang/Object;)Z",
											 QAndroidJniObject::fromString(item).object());
			}
			needSync = d->settings.callMethod<jboolean>("save", "(Ljava/lang/String;Ljava/util/Set;)Z",
														QAndroidJniObject::fromString(key).object(),
														hashSet.object());
			break;
		} else {
			auto ok = false;
			if(value.canConvert(QMetaType::QString) &&
			   QVariant{static_cast<int>(QMetaType::QString)}.canConvert(value.userType())) {
				strVar = value;
				ok = strVar.convert(QMetaType::QString);
			}

			if(!ok) {
				QByteArray data;
				QDataStream stream{&data, QIODevice::WriteOnly};
				stream << value;
				strVar = QStringLiteral("__qtmvvm_variant<%1>{%2}")
						 .arg(stream.version())
						 .arg(QString::fromUtf8(data.toBase64()));
			}
			Q_FALLTHROUGH();
		}
	case QMetaType::QString:
		needSync = d->settings.callMethod<jboolean>("save", "(Ljava/lang/String;Ljava/lang/String;)Z",
													QAndroidJniObject::fromString(key).object(),
													QAndroidJniObject::fromString((strVar.isValid() ? strVar : value).toString()).object());
		break;
	}

	if(needSync)
		QMetaObject::invokeMethod(this, "sync", Qt::QueuedConnection);
	emit entryChanged(key, value);
}

void AndroidSettingsAccessor::remove(const QString &key)
{
	if(!d->settings.isValid())
		return;

	QAndroidJniExceptionCleaner cleaner{QAndroidJniExceptionCleaner::OutputMode::Verbose};
	auto needSync = d->settings.callMethod<jboolean>("remove", "(Ljava/lang/String;)Z",
													 QAndroidJniObject::fromString(key).object());
	if(needSync)
		QMetaObject::invokeMethod(this, "sync", Qt::QueuedConnection);
	emit entryRemoved(key);
}

void AndroidSettingsAccessor::sync()
{
	QAndroidJniExceptionCleaner cleaner{QAndroidJniExceptionCleaner::OutputMode::Verbose};
	d->settings.callMethod<void>("sync");
}

void AndroidSettingsAccessor::changeCallback(const QString &key)
{
	auto value = load(key, {});
	if(contains(key))
		emit entryChanged(key, load(key));
	else
		emit entryRemoved(key);
}

// ------------- private implementation -------------

AndroidSettingsAccessorPrivate::AndroidSettingsAccessorPrivate(AndroidSettingsAccessor *q_ptr) :
	q{q_ptr}
{
	setup();
	QAndroidJniExceptionCleaner cleaner{QAndroidJniExceptionCleaner::OutputMode::Verbose};
	settings = QAndroidJniObject {
		"de/skycoder42/qtmvvm/core/AndroidSettingsAccessor",
		"(Landroid/content/Context;J)V",
		QtAndroid::androidContext().object(),
		reinterpret_cast<jlong>(this)
	};
}

AndroidSettingsAccessorPrivate::AndroidSettingsAccessorPrivate(AndroidSettingsAccessor *q_ptr, const QString &file, AndroidSettingsAccessor::Mode mode) :
	q{q_ptr}
{
	setup();
	QAndroidJniExceptionCleaner cleaner{QAndroidJniExceptionCleaner::OutputMode::Verbose};
	settings = QAndroidJniObject {
		"de/skycoder42/qtmvvm/core/AndroidSettingsAccessor",
		"(Landroid/content/Context;Ljava/lang/String;IJ)V",
		QtAndroid::androidContext().object(),
		QAndroidJniObject::fromString(file).object(),
		static_cast<jint>(mode),
		reinterpret_cast<jlong>(this)
	};
}

void AndroidSettingsAccessorPrivate::setup()
{
	varRegex = QRegularExpression {
		QStringLiteral(R"__(^__qtmvvm_variant<(\d+)>{(.*)}$)__"),
		QRegularExpression::OptimizeOnFirstUsageOption | QRegularExpression::DotMatchesEverythingOption
	};
}

QVariant AndroidSettingsAccessorPrivate::convertFromJava(const QAndroidJniObject &object)
{
	QAndroidJniExceptionCleaner cleaner{QAndroidJniExceptionCleaner::OutputMode::Verbose};
	QAndroidJniEnvironment env;

	auto stringClass = QAndroidJniObject::fromLocalRef(env->FindClass("java/lang/String"));
	auto intClass = QAndroidJniObject::fromLocalRef(env->FindClass("java/lang/Integer"));
	auto longClass = QAndroidJniObject::fromLocalRef(env->FindClass("java/lang/Long"));
	auto floatClass = QAndroidJniObject::fromLocalRef(env->FindClass("java/lang/Float"));
	auto boolClass = QAndroidJniObject::fromLocalRef(env->FindClass("java/lang/Boolean"));
	auto setClass = QAndroidJniObject::fromLocalRef(env->FindClass("java/util/Set"));

	if(env->IsInstanceOf(object.object(), intClass.object<jclass>()))
		return object.callMethod<jint>("intValue","()I");
	else if(env->IsInstanceOf(object.object(), longClass.object<jclass>()))
		return static_cast<qlonglong>(object.callMethod<jlong>("longValue","()J"));
	else if(env->IsInstanceOf(object.object(), floatClass.object<jclass>()))
		return object.callMethod<jfloat>("floatValue","()F");
	else if(env->IsInstanceOf(object.object(), boolClass.object<jclass>()))
		return QVariant::fromValue<bool>(object.callMethod<jboolean>("booleanValue","()Z"));
	else if(env->IsInstanceOf(object.object(), setClass.object<jclass>())) {
		QSet<QString> set;
		set.reserve(object.callMethod<jint>("size"));
		auto iterator = object.callObjectMethod("iterator", "()Ljava/util/Iterator;");
		if(!iterator.isValid())
			return {};
		while(iterator.callMethod<jboolean>("hasNext"))
			set.insert(iterator.callObjectMethod("next", "()Ljava/lang/Object;").toString());
		return QVariant::fromValue(set);
	} else if(env->IsInstanceOf(object.object(), stringClass.object<jclass>())) {
		auto str = object.toString();
		auto match = varRegex.match(str);
		if(match.hasMatch()) {
			auto data = QByteArray::fromBase64(match.captured(2).toUtf8());
			QDataStream stream{data};
			stream.setVersion(match.captured(1).toInt());
			QVariant result;
			stream.startTransaction();
			stream >> result;
			if(stream.commitTransaction())
				return result;
			else
				return {};
		} else
			return str;
	} else {
		logWarning() << "Unknown JAVA-Type in shared preferences";
		return {};
	}
}

void AndroidSettingsAccessorPrivate::dataChangedCallback(const QString &key)
{
	auto value = q->load(key, {});
	if(q->contains(key))
		emit q->entryChanged(key, q->load(key));
	else
		emit q->entryRemoved(key);
}

extern "C" {

JNIEXPORT void JNICALL Java_de_skycoder42_qtmvvm_core_AndroidSettingsAccessor_callback(JNIEnv */*env*/, jobject /*obj*/, jlong address, jobject key)
{
	auto self = reinterpret_cast<AndroidSettingsAccessorPrivate*>(address);
	QMetaObject::invokeMethod(self->q, "changeCallback", Qt::QueuedConnection,
							  Q_ARG(QString, QAndroidJniObject{key}.toString()));
}

}

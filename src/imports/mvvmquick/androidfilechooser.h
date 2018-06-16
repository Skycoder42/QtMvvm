#ifndef QTMVVM_ANDROIDFILECHOOSER_H
#define QTMVVM_ANDROIDFILECHOOSER_H

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QVariant>

#include <QtAndroidExtras/QAndroidActivityResultReceiver>
#include <QtAndroidExtras/QAndroidJniObject>

#ifdef DOXYGEN_RUN
namespace de::skycoder42::QtMvvm::Quick {

/*! @brief A QML class access the native file chooser on android
 *
 * @extends QtQml.QtObject
 * @since 1.0
 *
 * @warning Available on android only!
 *
 * A C++ wrapper around the android java API to access the native file chooser. It can only
 * be used on Android, and serves as the base for the android implementation of the file and
 * folder dialogs.
 *
 * @sa FileDialog, FolderDialog
 */
class FileChooser : public QtObject
#else
namespace QtMvvm {

class AndroidFileChooser : public QObject, public QAndroidActivityResultReceiver
#endif
{
	Q_OBJECT

	/*! @brief The activity title/description of the file chooser
	 *
	 * @default{<i>Empty</i>}
	 *
	 * @accessors{
	 *	@memberAc{title}
	 *  @notifyAc{titleChanged()}
	 * }
	 */
	Q_PROPERTY(QString title MEMBER _title NOTIFY titleChanged)

	/*! @brief The URL of the place to show the dialog from
	 *
	 * @default{<i>Empty</i>}
	 *
	 * @accessors{
	 *	@memberAc{folderUrl}
	 *  @notifyAc{folderUrlChanged()}
	 * }
	 */
	Q_PROPERTY(QUrl folderUrl MEMBER _folderUrl NOTIFY folderUrlChanged)
	/*! @brief The type of URL to get
	 *
	 * @default{`FileChooser::OpenDocument`}
	 *
	 * Basically the open mode to get a fitting URL for.
	 *
	 * @accessors{
	 *	@memberAc{type}
	 *  @notifyAc{typeChanged()}
	 * }
	 *
	 * @sa FileChooser::ChooserType
	 */
	Q_PROPERTY(ChooserType type MEMBER _type NOTIFY typeChanged)
	/*! @brief A list of acceptable mimetypes. Can contain wildcards
	 *
	 * @default{`["*@/*"]` (with just 1 slash instead of `@/`)}
	 *
	 * @accessors{
	 *	@memberAc{mimeTypes}
	 *  @notifyAc{mimeTypesChanged()}
	 * }
	 */
	Q_PROPERTY(QStringList mimeTypes MEMBER _mimeTypes NOTIFY mimeTypesChanged)
	/*! @brief Additional flags to configure the chooser
	 *
	 * @default{`FileChooser::OpenableFlag | FileChooser::AlwaysGrantWriteFlag`}
	 *
	 * @accessors{
	 *	@memberAc{chooserFlags}
	 *  @notifyAc{chooserFlagsChanged()}
	 * }
	 *
	 * @sa FileChooser::ChooserFlag
	 */
	Q_PROPERTY(ChooserFlags chooserFlags READ chooserFlags WRITE setChooserFlags NOTIFY chooserFlagsChanged)

	/*! @brief The chooser result url(s) the user selected
	 *
	 * @default{<i>Invalid</i>}
	 *
	 * Can either be a single QUrl or a QList<QUrl>. The list is only returned when
	 * FileChooser::OpenMultipleDocuments is used as FileChooser::type. For all other cases
	 * a single QUrl is returned.
	 *
	 * @accessors{
	 *	@memberAc{folderUrl}
	 *  @notifyAc{folderUrlChanged()}
	 *  @readonlyAc
	 * }
	 */
	Q_PROPERTY(QVariant result READ result NOTIFY resultChanged)

public:
	//! The different modes the chooser can return urls for
	enum ChooserType {
		GetContent = 0, //!< Get a non-permanent, read only content url
		OpenDocument = 1, //!< Get a permanent content url
		OpenMultipleDocuments = 2, //!< Get a list of permanent content urls
		CreateDocument = 3, //!< Create a new permanent content url
		OpenDocumentTree = 4 //!< Get a URL to content folder
	};
	Q_ENUM(ChooserType)

	//! Extra flags to configure how to open the URLs
	enum ChooserFlag {
		OpenableFlag = 0x01, //!< The returned URL must be openable (for reading)
		LocalOnlyFlag = 0x02, //!< Only local files are allowed
		AlwaysGrantWriteFlag = 0x04, //!< Always allow writing, not only for FileChooser::CreateDocument
		PersistPermissionsFlag = 0x08 //!< Persist the permission to access the content across reboots
	};
	Q_DECLARE_FLAGS(ChooserFlags, ChooserFlag)
	Q_FLAG(ChooserFlags)

	//! @private
	explicit AndroidFileChooser(QObject *parent = nullptr);
	~AndroidFileChooser() override;

	//! @private
	ChooserFlags chooserFlags() const;
	//! @private
	QVariant result() const;

#ifdef DOXYGEN_RUN
public:
#else
public Q_SLOTS:
#endif
	//! Opens the file chooser by sending the show intent
	void open();

	//! @private
	void setChooserFlags(ChooserFlags chooserFlags);

Q_SIGNALS:
	//! Emitted when the user select a url (or multiple)
	void accepted();
	//! Emitted when the user cancels without selecting a url
	void rejected();

	//! @notifyAcFn{FileChooser::title}
	void titleChanged(const QString &title);
	//! @notifyAcFn{FileChooser::folderUrl}
	void folderUrlChanged(const QUrl &folderUrl);
	//! @notifyAcFn{FileChooser::type}
	void typeChanged(ChooserType type);
	//! @notifyAcFn{FileChooser::mimeTypes}
	void mimeTypesChanged(const QStringList &mimeTypes);
	//! @notifyAcFn{FileChooser::chooserFlags}
	void chooserFlagsChanged(ChooserFlags chooserFlags);
	//! @notifyAcFn{FileChooser::result}
	void resultChanged(QVariant result);

protected:
	//! @private
	void handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data) override;

private:
	const static int RequestCodeNormal = 0x1091c657;
	const static int RequestCodePersist = 0x1091c658;

	QString _title;
	QUrl _folderUrl;
	ChooserType _type;
	QStringList _mimeTypes;
	ChooserFlags _flags;

	bool _active;
	QVariant _result;

	QAndroidJniObject createGetIntent();
	QAndroidJniObject createOpenIntent();
	QAndroidJniObject createOpenMultipleIntent();
	QAndroidJniObject createSaveIntent();
	QAndroidJniObject createOpenTreeIntent();

	void setupBasic(QAndroidJniObject &intent, bool asTree = false);
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(QtMvvm::AndroidFileChooser::ChooserFlags)

#endif // QTMVVM_ANDROIDFILECHOOSER_H

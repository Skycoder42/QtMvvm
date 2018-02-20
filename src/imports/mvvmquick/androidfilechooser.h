#ifndef QTMVVM_ANDROIDFILECHOOSER_H
#define QTMVVM_ANDROIDFILECHOOSER_H

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QVariant>

#include <QtAndroidExtras/QAndroidActivityResultReceiver>
#include <QtAndroidExtras/QAndroidJniObject>

namespace QtMvvm {

class AndroidFileChooser : public QObject, public QAndroidActivityResultReceiver
{
	Q_OBJECT

	Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)

	Q_PROPERTY(QUrl folderUrl READ folderUrl WRITE setFolderUrl NOTIFY folderUrlChanged)
	Q_PROPERTY(ChooserType type READ type WRITE setType NOTIFY typeChanged)
	Q_PROPERTY(QStringList mimeTypes READ mimeTypes WRITE setMimeTypes NOTIFY mimeTypesChanged)
	Q_PROPERTY(ChooserFlags chooserFlags READ chooserFlags WRITE setChooserFlags NOTIFY chooserFlagsChanged)

	Q_PROPERTY(QVariant result READ result NOTIFY resultChanged)

public:
	enum ChooserType {
		GetContent = 0,
		OpenDocument = 1,
		OpenMultipleDocuments = 2,
		CreateDocument = 3,
		OpenDocumentTree = 4
	};
	Q_ENUM(ChooserType)

	enum ChooserFlag {
		OpenableFlag = 0x01,
		LocalOnlyFlag = 0x02,
		AlwaysGrantWriteFlag = 0x04,
		PersistPermissionsFlag = 0x08
	};
	Q_DECLARE_FLAGS(ChooserFlags, ChooserFlag)
	Q_FLAG(ChooserFlags)

	explicit AndroidFileChooser(QObject *parent = nullptr);
	~AndroidFileChooser();

	QString title() const;
	QUrl folderUrl() const;
	ChooserType type() const;
	QStringList mimeTypes() const;
	ChooserFlags chooserFlags() const;

	QVariant result() const;

public Q_SLOTS:
	void open();

	void setTitle(const QString &title);
	void setFolderUrl(const QUrl &folderUrl);
	void setType(ChooserType type);
	void setMimeTypes(const QStringList &mimeTypes);
	void setChooserFlags(ChooserFlags chooserFlags);

Q_SIGNALS:
	void accepted();
	void rejected();

	void titleChanged(const QString &title);
	void folderUrlChanged(const QUrl &folderUrl);
	void typeChanged(ChooserType type);
	void mimeTypesChanged(const QStringList &mimeTypes);
	void chooserFlagsChanged(ChooserFlags chooserFlags);

	void resultChanged(QVariant result);

protected:
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

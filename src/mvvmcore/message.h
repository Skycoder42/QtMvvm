#ifndef QTMVVM_MESSAGE_H
#define QTMVVM_MESSAGE_H

#include <functional>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>

#include "QtMvvmCore/qtmvvmcore_global.h"

namespace QtMvvm {

class MessageConfigPrivate;
class Q_MVVMCORE_EXPORT MessageConfig
{
	Q_GADGET
	Q_DECLARE_TR_FUNCTIONS(MessageConfig)

	Q_PROPERTY(QByteArray type READ type WRITE setType)
	Q_PROPERTY(QByteArray subType READ subType WRITE setSubType RESET resetSubType)

	Q_PROPERTY(QString title READ title WRITE setTitle)
	Q_PROPERTY(QString text READ text WRITE setText)
	Q_PROPERTY(StandardButtons buttons READ buttons WRITE setButtons RESET resetButtons)
	Q_PROPERTY(QVariantMap buttonTexts READ buttonTextsMap WRITE setButtonTextsMap RESET resetButtons)

	Q_PROPERTY(QVariant defaultValue READ defaultValue WRITE setDefaultValue)
	Q_PROPERTY(QVariantMap viewProperties READ viewProperties WRITE setViewProperties)

public:
	enum StandardButton {
		// keep this in sync with QPlatformDialogHelper::StandardButton
		NoButton           = 0x00000000,
		Ok                 = 0x00000400,
		Save               = 0x00000800,
		SaveAll            = 0x00001000,
		Open               = 0x00002000,
		Yes                = 0x00004000,
		YesToAll           = 0x00008000,
		No                 = 0x00010000,
		NoToAll            = 0x00020000,
		Abort              = 0x00040000,
		Retry              = 0x00080000,
		Ignore             = 0x00100000,
		Close              = 0x00200000,
		Cancel             = 0x00400000,
		Discard            = 0x00800000,
		Help               = 0x01000000,
		Apply              = 0x02000000,
		Reset              = 0x04000000,
		RestoreDefaults    = 0x08000000
	};
	Q_DECLARE_FLAGS(StandardButtons, StandardButton)
	Q_FLAG(StandardButtons)

	static const QByteArray TypeMessageBox;
	static const QByteArray TypeInputDialog;
	static const QByteArray TypeFileDialog;

	static const QByteArray SubTypeInformation;
	static const QByteArray SubTypeWarning;
	static const QByteArray SubTypeCritical;
	static const QByteArray SubTypeQuestion;
	static const QByteArray SubTypeAbout;

	static const QByteArray SubTypeDir;
	static const QByteArray SubTypeOpenFile;
	static const QByteArray SubTypeOpenFiles;
	static const QByteArray SubTypeSaveFile;

	MessageConfig(const QByteArray &type = TypeMessageBox, const QByteArray &subType = {});
	MessageConfig(const MessageConfig &other);
	~MessageConfig();

	MessageConfig &operator=(const MessageConfig &other);

	QByteArray type() const;
	QByteArray subType() const;
	QString title() const;
	QString text() const;
	StandardButtons buttons() const;
	QHash<StandardButton, QString> buttonTexts() const;
	QByteArray inputType() const;
	QVariant defaultValue() const;
	QVariantMap viewProperties() const;

	MessageConfig &setType(const QByteArray &type);
	MessageConfig &setSubType(const QByteArray &subType);
	MessageConfig &setTitle(const QString &title);
	MessageConfig &setText(const QString &text);
	MessageConfig &setButtons(StandardButtons buttons);
	MessageConfig &setButtonTexts(const QHash<StandardButton, QString> &buttonTexts);
	MessageConfig &setButtonText(StandardButton button, const QString &text);
	MessageConfig &setDefaultValue(const QVariant &defaultValue);
	MessageConfig &setViewProperties(const QVariantMap &viewProperties);
	MessageConfig &setViewProperty(const QString &key, const QVariant &value);

	MessageConfig &resetSubType();
	MessageConfig &resetButtons();

private:
	QSharedDataPointer<MessageConfigPrivate> d;

	QVariantMap buttonTextsMap() const;
	void setButtonTextsMap(const QVariantMap &buttonTexts);
};

class MessageResultPrivate;
class Q_MVVMCORE_EXPORT MessageResult : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QVariant result READ result WRITE setResult)
	Q_PROPERTY(bool autoDelete READ autoDelete WRITE setAutoDelete NOTIFY autoDeleteChanged)

public:
	explicit MessageResult();
	~MessageResult();

	bool hasResult() const;
	QVariant result() const;
	bool autoDelete() const;

	//USE IN GUI ONLY
	//TODO document USE IN GUI ONLY in doc
	Q_INVOKABLE void setCloseTarget(QObject *closeObject, const QString &closeMethod);
	Q_INVOKABLE void setCloseTarget(QObject *closeObject, const QMetaMethod &closeMethod);
	Q_INVOKABLE void complete(QtMvvm::MessageConfig::StandardButton result);
	Q_INVOKABLE inline void complete(QtMvvm::MessageConfig::StandardButton result, const QVariant &resultValue) {
		setResult(resultValue);
		complete(result);
	}

public Q_SLOTS:
	void discardMessage();

	void setResult(QVariant result);
	void setAutoDelete(bool autoDelete);

Q_SIGNALS:
	void dialogDone(QtMvvm::MessageConfig::StandardButton result);

	void autoDeleteChanged(bool autoDelete);

private:
	QScopedPointer<MessageResultPrivate> d;
};

Q_MVVMCORE_EXPORT MessageResult *information(const QString &title,
											 const QString &text,
											 const QString &okText = {});
Q_MVVMCORE_EXPORT void information(const QString &title,
								   const QString &text,
								   QObject *scope,
								   const std::function<void()> &onResult,
								   const QString &okText = {});
Q_MVVMCORE_EXPORT void information(const QString &title,
								   const QString &text,
								   const std::function<void()> &onResult,
								   const QString &okText = {});

Q_MVVMCORE_EXPORT MessageResult *question(const QString &title,
										  const QString &text,
										  const QString &yesText = {},
										  const QString &noText = {});
Q_MVVMCORE_EXPORT void question(const QString &title,
								const QString &text,
								QObject *scope,
								const std::function<void(bool)> &onResult,
								const QString &yesText = {},
								const QString &noText = {});
Q_MVVMCORE_EXPORT void question(const QString &title,
								const QString &text,
								const std::function<void(bool)> &onResult,
								const QString &yesText = {},
								const QString &noText = {});

Q_MVVMCORE_EXPORT MessageResult *warning(const QString &title,
										 const QString &text,
										 const QString &okText = {});
Q_MVVMCORE_EXPORT void warning(const QString &title,
							   const QString &text,
							   QObject *scope,
							   const std::function<void()> &onResult,
							   const QString &okText = {});
Q_MVVMCORE_EXPORT void warning(const QString &title,
							   const QString &text,
							   const std::function<void()> &onResult,
							   const QString &okText = {});

Q_MVVMCORE_EXPORT MessageResult *critical(const QString &title,
										  const QString &text,
										  const QString &okText = {});
Q_MVVMCORE_EXPORT void critical(const QString &title,
								const QString &text,
								QObject *scope,
								const std::function<void()> &onResult,
								const QString &okText = {});
Q_MVVMCORE_EXPORT void critical(const QString &title,
								const QString &text,
								const std::function<void()> &onResult,
								const QString &okText = {});

Q_MVVMCORE_EXPORT MessageResult *about(const QString &description,
									   const QUrl &websiteUrl = QUrl(),
									   const QString &licenseName = QString(),
									   const QUrl &licenseUrl = QUrl(),
									   const QString &companyName = QString(),
									   bool addQtVersion = true,
									   const QStringList &extraTopInfos = QStringList(),
									   const QString &extraBottomInfos = QString());

Q_MVVMCORE_EXPORT MessageResult *getInput(const QString &title,
										  const QString &text,
										  const char *inputType,
										  const QVariant &defaultValue = {},
										  const QVariantMap &viewProperties = {},
										  const QString &okText = {},
										  const QString &cancelText = {});
Q_MVVMCORE_EXPORT void getInput(const QString &title,
								const QString &text,
								const char *inputType,
								QObject *scope,
								const std::function<void(QVariant)> &onResult,
								const QVariant &defaultValue = {},
								const QVariantMap &viewProperties = {},
								const QString &okText = {},
								const QString &cancelText = {});
Q_MVVMCORE_EXPORT void getInput(const QString &title,
								const QString &text,
								const char *inputType,
								const std::function<void(QVariant)> &onResult,
								const QVariant &defaultValue = {},
								const QVariantMap &viewProperties = {},
								const QString &okText = {},
								const QString &cancelText = {});

template <typename TEdit>
inline MessageResult *getInput(const QString &title,
							   const QString &text,
							   const QVariant &defaultValue = {},
							   const QVariantMap &viewProperties = {},
							   const QString &okText = {},
							   const QString &cancelText = {}) {
	return getInput(title, text, QMetaType::typeName(qMetaTypeId<TEdit>()), defaultValue, viewProperties, okText, cancelText);
}
template <typename TEdit>
inline void getInput(const QString &title,
					 const QString &text,
					 QObject *scope,
					 const std::function<void(TEdit, bool)> &onResult,
					 const QVariant &defaultValue = {},
					 const QVariantMap &viewProperties = {},
					 const QString &okText = {},
					 const QString &cancelText = {}) {
	getInput(title, text, QMetaType::typeName(qMetaTypeId<TEdit>()), scope, [onResult](QVariant v) {
		onResult(v.template value<TEdit>(), v.isValid());
	}, defaultValue, viewProperties, okText, cancelText);
}
template <typename TEdit>
inline void getInput(const QString &title,
					 const QString &text,
					 const std::function<void(TEdit, bool)> &onResult,
					 const QVariant &defaultValue = {},
					 const QVariantMap &viewProperties = {},
					 const QString &okText = {},
					 const QString &cancelText = {}) {
	getInput(title, text, QMetaType::typeName(qMetaTypeId<TEdit>()), [onResult](QVariant v) {
		onResult(v.template value<TEdit>(), v.isValid());
	}, defaultValue, viewProperties, okText, cancelText);
}

Q_MVVMCORE_EXPORT MessageResult *getExistingDirectory(const QString &title = {},
													  const QUrl &dir = {});
Q_MVVMCORE_EXPORT void getExistingDirectory(QObject *scope,
											const std::function<void(QUrl)> &onResult,
											const QString &title = {},
											const QUrl &dir = {});
Q_MVVMCORE_EXPORT void getExistingDirectory(const std::function<void(QUrl)> &onResult,
											const QString &title = {},
											const QUrl &dir = {});

Q_MVVMCORE_EXPORT MessageResult *getOpenFile(const QString &title = {},
											 const QStringList &supportedMimeTypes = {},
											 const QUrl &dir = {});
Q_MVVMCORE_EXPORT void getOpenFile(QObject *scope,
								   const std::function<void(QUrl)> &onResult,
								   const QString &title = {},
								   const QStringList &supportedMimeTypes = {},
								   const QUrl &dir = {});
Q_MVVMCORE_EXPORT void getOpenFile(const std::function<void(QUrl)> &onResult,
								   const QString &title = {},
								   const QStringList &supportedMimeTypes = {},
								   const QUrl &dir = {});

Q_MVVMCORE_EXPORT MessageResult *getOpenFiles(const QString &title = {},
											  const QStringList &supportedMimeTypes = {},
											  const QUrl &dir = {});
Q_MVVMCORE_EXPORT void getOpenFiles(QObject *scope,
									const std::function<void(QList<QUrl>)> &onResult,
									const QString &title = {},
									const QStringList &supportedMimeTypes = {},
									const QUrl &dir = {});
Q_MVVMCORE_EXPORT void getOpenFiles(const std::function<void(QList<QUrl>)> &onResult,
									const QString &title = {},
									const QStringList &supportedMimeTypes = {},
									const QUrl &dir = {});

Q_MVVMCORE_EXPORT MessageResult *getSaveFile(const QString &title = {},
											 const QStringList &supportedMimeTypes = {},
											 const QUrl &dir = {});
Q_MVVMCORE_EXPORT void getSaveFile(QObject *scope,
								   const std::function<void(QUrl)> &onResult,
								   const QString &title = {},
								   const QStringList &supportedMimeTypes = {},
								   const QUrl &dir = {});
Q_MVVMCORE_EXPORT void getSaveFile(const std::function<void(QUrl)> &onResult,
								   const QString &title = {},
								   const QStringList &supportedMimeTypes = {},
								   const QUrl &dir = {});

}

Q_DECLARE_METATYPE(QtMvvm::MessageConfig)
Q_DECLARE_METATYPE(QtMvvm::MessageConfig::StandardButton)
Q_DECLARE_TYPEINFO(QtMvvm::MessageConfig, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(QtMvvm::MessageResult*)
Q_DECLARE_OPERATORS_FOR_FLAGS(QtMvvm::MessageConfig::StandardButtons)

#endif // QTMVVM_MESSAGE_H

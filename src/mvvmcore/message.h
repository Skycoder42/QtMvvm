#ifndef QTMVVM_MESSAGE_H
#define QTMVVM_MESSAGE_H

#include <functional>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>

#include <QtGui/qcolor.h>

#include "QtMvvmCore/qtmvvmcore_global.h"

namespace QtMvvm {

class MessageConfigPrivate;
//! A configuration for a simple dialog to be shown from the core code
class Q_MVVMCORE_EXPORT MessageConfig
{
	Q_GADGET
	Q_DECLARE_TR_FUNCTIONS(MessageConfig)

	//! The general type of dialog to be shown
	Q_PROPERTY(QByteArray type READ type WRITE setType)
	//! A subtype for the dialog to specify what exactly it should show
	Q_PROPERTY(QByteArray subType READ subType WRITE setSubType RESET resetSubType)

	//! The dialog title or window title
	Q_PROPERTY(QString title READ title WRITE setTitle)
	//! The primary text/message to be displayed
	Q_PROPERTY(QString text READ text WRITE setText)
	//! The available buttons a user may press
	Q_PROPERTY(StandardButtons buttons READ buttons WRITE setButtons RESET resetButtons)
	//! A map of name overwrites for the texts shown on buttons
	Q_PROPERTY(QVariantMap buttonTexts READ buttonTextsMap WRITE setButtonTextsMap RESET resetButtons)

	//! A default value for dialogs that take some kind of input
	Q_PROPERTY(QVariant defaultValue READ defaultValue WRITE setDefaultValue)
	//! Additional properties to be set on the created views
	Q_PROPERTY(QVariantMap viewProperties READ viewProperties WRITE setViewProperties)

public:
	//! The Possible Buttons a dialog can have
	enum StandardButton {
		// keep this in sync with QPlatformDialogHelper::StandardButton
		NoButton           = 0x00000000, //!< Equivalent to QMessageBox::NoButton
		Ok                 = 0x00000400, //!< Equivalent to QMessageBox::Ok
		Save               = 0x00000800, //!< Equivalent to QMessageBox::Save
		SaveAll            = 0x00001000, //!< Equivalent to QMessageBox::SaveAll
		Open               = 0x00002000, //!< Equivalent to QMessageBox::Open
		Yes                = 0x00004000, //!< Equivalent to QMessageBox::Yes
		YesToAll           = 0x00008000, //!< Equivalent to QMessageBox::YesToAll
		No                 = 0x00010000, //!< Equivalent to QMessageBox::No
		NoToAll            = 0x00020000, //!< Equivalent to QMessageBox::NoToAll
		Abort              = 0x00040000, //!< Equivalent to QMessageBox::Abort
		Retry              = 0x00080000, //!< Equivalent to QMessageBox::Retry
		Ignore             = 0x00100000, //!< Equivalent to QMessageBox::Ignore
		Close              = 0x00200000, //!< Equivalent to QMessageBox::Close
		Cancel             = 0x00400000, //!< Equivalent to QMessageBox::Cancel
		Discard            = 0x00800000, //!< Equivalent to QMessageBox::Discard
		Help               = 0x01000000, //!< Equivalent to QMessageBox::Help
		Apply              = 0x02000000, //!< Equivalent to QMessageBox::Apply
		Reset              = 0x04000000, //!< Equivalent to QMessageBox::Reset
		RestoreDefaults    = 0x08000000 //!< Equivalent to QMessageBox::RestoreDefaults
	};
	Q_DECLARE_FLAGS(StandardButtons, StandardButton)
	Q_FLAG(StandardButtons)

	/**
	 * @name Possible standard values for MessageConfig::type
	 * @{
	 */
	//! @brief A type to show a standard message box
	static const QByteArray TypeMessageBox;
	//! A type to show a generic input dialog
	static const QByteArray TypeInputDialog;
	//! A type to show a generic file dialog
	static const QByteArray TypeFileDialog;
	//! A type to show a generic color dialog
	static const QByteArray TypeColorDialog;
	//! A type to show a generic progress dialog
	static const QByteArray TypeProgressDialog;
	//! @}

	/**
	 * @name Possible values for MessageConfig::subType when using the type MessageConfig::TypeMessageBox
	 * @{
	 */
	//! @brief A subType to show an information message
	static const QByteArray SubTypeInformation;
	//! A subType to show a warning message
	static const QByteArray SubTypeWarning;
	//! A subType to show a critical error message
	static const QByteArray SubTypeCritical;
	//! A subType to show a question message
	static const QByteArray SubTypeQuestion;
	//! A subType to show an about dialog
	static const QByteArray SubTypeAbout;
	//! @}

	/**
	 * @name Possible values for MessageConfig::subType when using the type MessageConfig::TypeFileDialog
	 * @{
	 */
	//! @brief A subType to show an open directory dialog
	static const QByteArray SubTypeDir;
	//! A subType for to show an open file dialog
	static const QByteArray SubTypeOpenFile;
	//! A subType for to show an open files dialog
	static const QByteArray SubTypeOpenFiles;
	//! A subType for to show a save file dialog
	static const QByteArray SubTypeSaveFile;
	//! @}

	/**
	 * @name Possible values for MessageConfig::subType when using the type MessageConfig::TypeColorDialog
	 * @{
	 */
	//! @brief A subType to show a color dialog without an alpha channel
	static const QByteArray SubTypeRgb;
	//! A subType to show a color dialog with an alpha channel
	static const QByteArray SubTypeArgb;
	//! @}

	/**
	 * @name Possible values for MessageConfig::subType when using the type MessageConfig::TypeProgressDialog
	 * @{
	 */
	//! @brief A subType to show a dialog with a progress bar
	static const QByteArray SubTypeProgress;
	//! A subType to show a dialog with a busy indicator
	static const QByteArray SubTypeBusy;
	//! @}

	//! Default constructor, can take a type and a subtype
	MessageConfig(const QByteArray &type = TypeMessageBox, const QByteArray &subType = {});
	//! Copy constructor
	MessageConfig(const MessageConfig &other);
	//! Move constructor
	MessageConfig(MessageConfig &&other) noexcept;
	~MessageConfig();

	//! Copy assignment operator
	MessageConfig &operator=(const MessageConfig &other);
	//! Move assignment operator
	MessageConfig &operator=(MessageConfig &&other) noexcept;

	//! @readAcFn{MessageConfig::type}
	QByteArray type() const;
	//! @readAcFn{MessageConfig::subType}
	QByteArray subType() const;
	//! @readAcFn{MessageConfig::title}
	QString title() const;
	//! @readAcFn{MessageConfig::text}
	QString text() const;
	//! @readAcFn{MessageConfig::buttons}
	StandardButtons buttons() const;
	//! @readAcFn{MessageConfig::buttonTexts}
	QHash<StandardButton, QString> buttonTexts() const;
	//! @readAcFn{MessageConfig::defaultValue}
	QVariant defaultValue() const;
	//! @readAcFn{MessageConfig::viewProperties}
	QVariantMap viewProperties() const;

	//! @writeAcFn{MessageConfig::type}
	MessageConfig &setType(const QByteArray &type);
	//! @writeAcFn{MessageConfig::subType}
	MessageConfig &setSubType(const QByteArray &subType);
	//! @writeAcFn{MessageConfig::title}
	MessageConfig &setTitle(const QString &title);
	//! @writeAcFn{MessageConfig::text}
	MessageConfig &setText(const QString &text);
	//! @writeAcFn{MessageConfig::buttons}
	MessageConfig &setButtons(StandardButtons buttons);
	//! @writeAcFn{MessageConfig::buttons}
	MessageConfig &addButton(StandardButton button);
	//! @writeAcFn{MessageConfig::buttonTexts}
	MessageConfig &setButtonTexts(const QHash<StandardButton, QString> &buttonTexts);
	//! @writeAcFn{MessageConfig::buttonTexts}
	MessageConfig &setButtonText(StandardButton button, const QString &text);
	//! @writeAcFn{MessageConfig::defaultValue}
	MessageConfig &setDefaultValue(const QVariant &defaultValue);
	//! @writeAcFn{MessageConfig::viewProperties}
	MessageConfig &setViewProperties(const QVariantMap &viewProperties);
	//! @writeAcFn{MessageConfig::viewProperties}
	MessageConfig &setViewProperty(const QString &key, const QVariant &value);

	//! @resetAcFn{MessageConfig::subType}
	MessageConfig &resetSubType();
	//! @resetAcFn{MessageConfig::buttons}
	MessageConfig &resetButtons();

private:
	QSharedDataPointer<MessageConfigPrivate> d;

	QVariantMap buttonTextsMap() const;
	void setButtonTextsMap(const QVariantMap &buttonTexts);
};

class MessageResultPrivate;
//! A result watcher to get the result once a dialog has finished
class Q_MVVMCORE_EXPORT MessageResult : public QObject
{
	Q_OBJECT

	//! The result for an input, if the dialog had any
	Q_PROPERTY(QVariant result READ result WRITE setResult)
	//! Specifies whether the object should delete itself after completition
	Q_PROPERTY(bool autoDelete READ autoDelete WRITE setAutoDelete NOTIFY autoDeleteChanged)

public:
	explicit MessageResult();
	~MessageResult() override;

	//! Checks if the dialog has a result
	bool hasResult() const;
	//! @readAcFn{MessageResult::result}
	QVariant result() const;
	//! @readAcFn{MessageResult::autoDelete}
	bool autoDelete() const;

	/**
	 * @name Presenter-Only methods
	 * @details The following methods should be used by the presenter only, not from the core
	 * @{
	 */
	//! @brief Sets an object and a method to be called to close the dialog
	Q_INVOKABLE void setCloseTarget(QObject *closeObject, const QString &closeMethod);
	//! @copydoc MessageResult::setCloseTarget(QObject *, const QString &)
	Q_INVOKABLE void setCloseTarget(QObject *closeObject, const QMetaMethod &closeMethod);
	//! Completes the dialog and tells the result that it is finished
	Q_INVOKABLE void complete(QtMvvm::MessageConfig::StandardButton button);
	//! @copybrief MessageResult::complete(QtMvvm::MessageConfig::StandardButton)
	Q_INVOKABLE inline void complete(QtMvvm::MessageConfig::StandardButton button, const QVariant &result) {
		setResult(result);
		complete(button);
	}
	//! @}

public Q_SLOTS:
	//! Tries to discard the dialog by closing it
	void discardMessage();

	//! @writeAcFn{MessageResult::result}
	void setResult(QVariant result);
	//! @writeAcFn{MessageResult::autoDelete}
	void setAutoDelete(bool autoDelete);

Q_SIGNALS:
	//! Is emitted as soon as the dialog was completed
	void dialogDone(QtMvvm::MessageConfig::StandardButton button, QPrivateSignal);

	//! @notifyAcFn{MessageResult::autoDelete}
	void autoDeleteChanged(bool autoDelete, QPrivateSignal);

private:
	QScopedPointer<MessageResultPrivate> d;
};

class ProgressControlPrivate;
//! A Helper class to control a generic progress dialog
class Q_MVVMCORE_EXPORT ProgressControl : public QObject
{
	Q_OBJECT

	//! Specifies whether the object should delete itself after completition
	Q_PROPERTY(bool autoDelete READ autoDelete WRITE setAutoDelete NOTIFY autoDeleteChanged)

	//! Specifies whether the dialog should show an indeterminate or a normal progress
	Q_PROPERTY(bool indeterminate READ isIndeterminate WRITE setIndeterminate NOTIFY indeterminateChanged)
	//! The minimum value of the progress bar
	Q_PROPERTY(int minimum READ minimum WRITE setMinimum NOTIFY minimumChanged)
	//! The maximum value of the progress bar
	Q_PROPERTY(int maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)
	//! The current value of the progress bar
	Q_PROPERTY(int progress READ progress WRITE setProgress NOTIFY progressChanged)

public:
	//! Constructor
	explicit ProgressControl(QObject *parent = nullptr);
	~ProgressControl() override;

	//! @readAcFn{ProgressControl::autoDelete}
	bool autoDelete() const;
	//! @readAcFn{ProgressControl::indeterminate}
	bool isIndeterminate() const;
	//! @readAcFn{ProgressControl::minimum}
	int minimum() const;
	//! @readAcFn{ProgressControl::maximum}
	int maximum() const;
	//! @readAcFn{ProgressControl::progress}
	int progress() const;

	/**
	 * @name Presenter-Only methods
	 * @details The following methods should be used by the presenter only, not from the core
	 * @{
	 */
	//! @brief Signals the core app that the user tried to cancel the progress
	Q_INVOKABLE void requestCancel(QtMvvm::MessageConfig::StandardButton btn);
	//! Signals the core app that the dialog was closed
	Q_INVOKABLE void notifyClosed();
	//! @}

public Q_SLOTS:
	//! Closes the dialog asynchronously
	void close();
	//! Updates the descriptive text displayed in the progress dialog
	void updateLabel(const QString &text);

	//! @writeAcFn{ProgressControl::autoDelete}
	void setAutoDelete(bool autoDelete);
	//! @writeAcFn{ProgressControl::indeterminate}
	void setIndeterminate(bool indeterminate);
	//! @writeAcFn{ProgressControl::minimum}
	void setMinimum(int minimum);
	//! @writeAcFn{ProgressControl::maximum}
	void setMaximum(int maximum);
	//! @writeAcFn{ProgressControl::progress}
	void setProgress(int progress);
	//! @writeAcFn{ProgressControl::progressPercent}
	void setProgress(double progressPercent);

Q_SIGNALS:
	//! @notifyAcFn{ProgressControl::autoDelete}
	void autoDeleteChanged(bool autoDelete, QPrivateSignal);
	//! @notifyAcFn{ProgressControl::indeterminate}
	void indeterminateChanged(bool indeterminate, QPrivateSignal);
	//! @notifyAcFn{ProgressControl::minimum}
	void minimumChanged(int minimum, QPrivateSignal);
	//! @notifyAcFn{ProgressControl::maximum}
	void maximumChanged(int maximum, QPrivateSignal);
	//! @notifyAcFn{ProgressControl::progress}
	void progressChanged(int progress, QPrivateSignal);

	//! Is emitted when the user canceled the dialog
	void canceled(QtMvvm::MessageConfig::StandardButton btn, QPrivateSignal);
	//! Is emitted after the dialog gui has been closed
	void closed(QPrivateSignal);

	/**
	 * @name Presenter-Only signals
	 * @details The following signals should be used by the presenter only, not from the core
	 * @{
	 */
	//! @brief Is emitted when the descriptive text in the dialog should be updated
	void changeLabel(const QString &text, QPrivateSignal);
	//! Is emitted when a closed was requested from the core code
	void closeRequested(QPrivateSignal);
	//! @}

private:
	QScopedPointer<ProgressControlPrivate> d;
};

/**
 * @name Methods to show simple messageboxes (MessageConfig::TypeMessageBox)
 * @{
 */
//! @brief A shortcut to show a simple information messagebox
Q_MVVMCORE_EXPORT MessageResult *information(const QString &title,
											 const QString &text,
											 const QString &okText = {});
//! @copybrief QtMvvm::information(const QString &, const QString &, const std::function<void()> &, const QString &)
Q_MVVMCORE_EXPORT void information(const QString &title,
								   const QString &text,
								   QObject *scope,
								   const std::function<void()> &onResult,
								   const QString &okText = {});
//! @copybrief QtMvvm::information(const QString &, const QString &, const QString &)
Q_MVVMCORE_EXPORT void information(const QString &title,
								   const QString &text,
								   const std::function<void()> &onResult,
								   const QString &okText = {});

//! A shortcut to show a simple question messagebox
Q_MVVMCORE_EXPORT MessageResult *question(const QString &title,
										  const QString &text,
										  const QString &yesText = {},
										  const QString &noText = {});
//! @copybrief QtMvvm::question(const QString &, const QString &, const std::function<void(bool)> &, const QString &, const QString &)
Q_MVVMCORE_EXPORT void question(const QString &title,
								const QString &text,
								QObject *scope,
								const std::function<void(bool)> &onResult,
								const QString &yesText = {},
								const QString &noText = {});
//! @copybrief QtMvvm::question(const QString &, const QString &, const QString &, const QString &)
Q_MVVMCORE_EXPORT void question(const QString &title,
								const QString &text,
								const std::function<void(bool)> &onResult,
								const QString &yesText = {},
								const QString &noText = {});

//! A shortcut to show a simple warning messagebox
Q_MVVMCORE_EXPORT MessageResult *warning(const QString &title,
										 const QString &text,
										 const QString &okText = {});
//! @copybrief QtMvvm::warning(const QString &, const QString &, const std::function<void()> &, const QString &)
Q_MVVMCORE_EXPORT void warning(const QString &title,
							   const QString &text,
							   QObject *scope,
							   const std::function<void()> &onResult,
							   const QString &okText = {});
//! @copybrief QtMvvm::warning(const QString &, const QString &, const QString &)
Q_MVVMCORE_EXPORT void warning(const QString &title,
							   const QString &text,
							   const std::function<void()> &onResult,
							   const QString &okText = {});

//! A shortcut to show a simple critical error messagebox
Q_MVVMCORE_EXPORT MessageResult *critical(const QString &title,
										  const QString &text,
										  const QString &okText = {});
//! @copybrief QtMvvm::critical(const QString &, const QString &, const std::function<void()> &, const QString &)
Q_MVVMCORE_EXPORT void critical(const QString &title,
								const QString &text,
								QObject *scope,
								const std::function<void()> &onResult,
								const QString &okText = {});
//! @copybrief QtMvvm::critical(const QString &, const QString &, const QString &)
Q_MVVMCORE_EXPORT void critical(const QString &title,
								const QString &text,
								const std::function<void()> &onResult,
								const QString &okText = {});

//! A shortcut to show a rich but simple about dialog
Q_MVVMCORE_EXPORT MessageResult *about(const QString &description,
									   const QUrl &websiteUrl = QUrl(),
									   const QString &licenseName = QString(),
									   const QUrl &licenseUrl = QUrl(),
									   const QString &companyName = QString(),
									   bool addQtVersion = true,
									   const QStringList &extraTopInfos = QStringList(),
									   const QString &extraBottomInfos = QString());
//! @}

/**
 * @name Methods to show simple input dialogs (MessageConfig::TypeInputDialog)
 * @{
 */
//! @brief A shortcut to show an input dialog based of a type name
Q_MVVMCORE_EXPORT MessageResult *getInput(const QString &title,
										  const QString &text,
										  const char *inputType,
										  const QVariant &defaultValue = {},
										  const QVariantMap &viewProperties = {},
										  const QString &okText = {},
										  const QString &cancelText = {});
//! @copybrief QtMvvm::getInput(const QString &, const QString &, const char *, const std::function<void(QVariant)> &, const QVariant &, const QVariantMap &, const QString &, const QString &)
Q_MVVMCORE_EXPORT void getInput(const QString &title,
								const QString &text,
								const char *inputType,
								QObject *scope,
								const std::function<void(QVariant)> &onResult,
								const QVariant &defaultValue = {},
								const QVariantMap &viewProperties = {},
								const QString &okText = {},
								const QString &cancelText = {});
//! @copybrief QtMvvm::getInput(const QString &, const QString &, const char *, const QVariant &, const QVariantMap &, const QString &, const QString &)
Q_MVVMCORE_EXPORT void getInput(const QString &title,
								const QString &text,
								const char *inputType,
								const std::function<void(QVariant)> &onResult,
								const QVariant &defaultValue = {},
								const QVariantMap &viewProperties = {},
								const QString &okText = {},
								const QString &cancelText = {});

//! A shortcut to show an input dialog based of a type as generic parameter
template <typename TEdit>
inline MessageResult *getInput(const QString &title,
							   const QString &text,
							   const TEdit &defaultValue = {},
							   const QVariantMap &viewProperties = {},
							   const QString &okText = {},
							   const QString &cancelText = {}) {
	return getInput(title, text, QMetaType::typeName(qMetaTypeId<TEdit>()), QVariant::fromValue(defaultValue), viewProperties, okText, cancelText);
}
//! @copybrief QtMvvm::getInput(const QString &, const QString &, const std::function<void(TEdit, bool)> &, const TEdit &, const QVariantMap &, const QString &, const QString &)
template <typename TEdit>
inline void getInput(const QString &title,
					 const QString &text,
					 QObject *scope,
					 const std::function<void(TEdit, bool)> &onResult,
					 const TEdit &defaultValue = {},
					 const QVariantMap &viewProperties = {},
					 const QString &okText = {},
					 const QString &cancelText = {}) {
	getInput(title, text, QMetaType::typeName(qMetaTypeId<TEdit>()), scope, [onResult](const QVariant &v) {
		onResult(v.template value<TEdit>(), v.isValid());
	}, QVariant::fromValue(defaultValue), viewProperties, okText, cancelText);
}
//! @copybrief QtMvvm::getInput(const QString &, const QString &, const TEdit &, const QVariantMap &, const QString &, const QString &)
template <typename TEdit>
inline void getInput(const QString &title,
					 const QString &text,
					 const std::function<void(TEdit, bool)> &onResult,
					 const TEdit &defaultValue = {},
					 const QVariantMap &viewProperties = {},
					 const QString &okText = {},
					 const QString &cancelText = {}) {
	getInput(title, text, QMetaType::typeName(qMetaTypeId<TEdit>()), [onResult](const QVariant &v) {
		onResult(v.template value<TEdit>(), v.isValid());
	}, QVariant::fromValue(defaultValue), viewProperties, okText, cancelText);
}
//! @}

/**
 * @name Methods to show simple file dialogs (MessageConfig::TypeFileDialog)
 * @{
 */
//! @brief A shortcut to show a file dialog to select an existing directory
Q_MVVMCORE_EXPORT MessageResult *getExistingDirectory(const QString &title = {},
													  const QUrl &dir = {});
//! @copybrief QtMvvm::getExistingDirectory(const std::function<void(QUrl)> &, const QString &, const QUrl &)
Q_MVVMCORE_EXPORT void getExistingDirectory(QObject *scope,
											const std::function<void(QUrl)> &onResult,
											const QString &title = {},
											const QUrl &dir = {});
//! @copybrief QtMvvm::getExistingDirectory(const QString &, const QUrl &)
Q_MVVMCORE_EXPORT void getExistingDirectory(const std::function<void(QUrl)> &onResult,
											const QString &title = {},
											const QUrl &dir = {});

//! A shortcut to show a file dialog to select an existing file
Q_MVVMCORE_EXPORT MessageResult *getOpenFile(const QString &title = {},
											 const QStringList &supportedMimeTypes = {},
											 const QUrl &dir = {});
//! @copybrief QtMvvm::getOpenFile(const std::function<void(QUrl)> &, const QString &, const QStringList &, const QUrl &)
Q_MVVMCORE_EXPORT void getOpenFile(QObject *scope,
								   const std::function<void(QUrl)> &onResult,
								   const QString &title = {},
								   const QStringList &supportedMimeTypes = {},
								   const QUrl &dir = {});
//! @copybrief QtMvvm::getOpenFile(const QString &, const QStringList &, const QUrl &)
Q_MVVMCORE_EXPORT void getOpenFile(const std::function<void(QUrl)> &onResult,
								   const QString &title = {},
								   const QStringList &supportedMimeTypes = {},
								   const QUrl &dir = {});

//! A shortcut to show a file dialog to select multiple existing files
Q_MVVMCORE_EXPORT MessageResult *getOpenFiles(const QString &title = {},
											  const QStringList &supportedMimeTypes = {},
											  const QUrl &dir = {});
//! @copybrief QtMvvm::getOpenFiles(const std::function<void(QList<QUrl>)> &, const QString &, const QStringList &, const QUrl &)
Q_MVVMCORE_EXPORT void getOpenFiles(QObject *scope,
									const std::function<void(QList<QUrl>)> &onResult,
									const QString &title = {},
									const QStringList &supportedMimeTypes = {},
									const QUrl &dir = {});
//! @copybrief QtMvvm::getOpenFiles(const QString &, const QStringList &, const QUrl &)
Q_MVVMCORE_EXPORT void getOpenFiles(const std::function<void(QList<QUrl>)> &onResult,
									const QString &title = {},
									const QStringList &supportedMimeTypes = {},
									const QUrl &dir = {});

//! A shortcut to show a file dialog to select a new file for saving
Q_MVVMCORE_EXPORT MessageResult *getSaveFile(const QString &title = {},
											 const QStringList &supportedMimeTypes = {},
											 const QUrl &dir = {});
//! @copybrief QtMvvm::getSaveFile(const std::function<void(QUrl)> &, const QString &, const QStringList &, const QUrl &)
Q_MVVMCORE_EXPORT void getSaveFile(QObject *scope,
								   const std::function<void(QUrl)> &onResult,
								   const QString &title = {},
								   const QStringList &supportedMimeTypes = {},
								   const QUrl &dir = {});
//! @copybrief QtMvvm::getSaveFile(const QString &, const QStringList &, const QUrl &)
Q_MVVMCORE_EXPORT void getSaveFile(const std::function<void(QUrl)> &onResult,
								   const QString &title = {},
								   const QStringList &supportedMimeTypes = {},
								   const QUrl &dir = {});
//! @}

/**
 * @name Methods to show simple color dialogs (MessageConfig::TypeColorDialog)
 * @{
 */
//! @brief A shortcut to show a color dialog to select a color
Q_MVVMCORE_EXPORT MessageResult *getColor(const QString &title = {},
										  const QColor &color = {},
										  bool argb = false);
//! @copybrief QtMvvm::getColor(const std::function<void(QColor)> &, const QString &, const QColor &, bool)
Q_MVVMCORE_EXPORT void getColor(QObject *scope,
								const std::function<void(QColor)> &onResult,
								const QString &title = {},
								const QColor &color = {},
								bool argb = false);
//! @copybrief QtMvvm::getColor(const QString &, const QColor &, bool)
Q_MVVMCORE_EXPORT void getColor(const std::function<void(QColor)> &onResult,
								const QString &title = {},
								const QColor &color = {},
								bool argb = false);
//! @}

/**
 * @name Methods to show simple progress dialogs (MessageConfig::TypeProgressDialog)
 * @{
 */
//! @brief A shortcut to show a general progress dialog
Q_MVVMCORE_EXPORT MessageResult *showProgress(const QString &title,
											  const QString &label,
											  ProgressControl *control,
											  bool allowCancel = true,
											  bool isBusy = false,
											  const QString &cancelText = {});

//! @copybrief QtMvvm::showProgress(const QString &, const QString &, int, int, bool, int, const QString &)
Q_MVVMCORE_EXPORT ProgressControl *showProgress(QObject *scope,
												const QString &title = {},
												const QString &label = {},
												int maximum = 100,
												int minimum = 0,
												bool allowCancel = true,
												int value = 0,
												const QString &cancelText = {});
//! A shortcut to show a standard progress dialog
Q_MVVMCORE_EXPORT ProgressControl *showProgress(const QString &title = {},
												const QString &label = {},
												int maximum = 100,
												int minimum = 0,
												bool allowCancel = true,
												int value = 0,
												const QString &cancelText = {});

//! @copybrief QtMvvm::showIndeterminateProgress(const QString &, const QString &, bool, const QString &)
Q_MVVMCORE_EXPORT ProgressControl *showIndeterminateProgress(QObject *scope,
															 const QString &title = {},
															 const QString &label = {},
															 bool allowCancel = true,
															 const QString &cancelText = {});
//! A shortcut to show an indetermiante progress dialog
Q_MVVMCORE_EXPORT ProgressControl *showIndeterminateProgress(const QString &title = {},
															 const QString &label = {},
															 bool allowCancel = true,
															 const QString &cancelText = {});

//! @copybrief QtMvvm::showBusy(const QString &, const QString &, bool, const QString &)
Q_MVVMCORE_EXPORT ProgressControl *showBusy(QObject *scope,
											const QString &title = {},
											const QString &label = {},
											bool allowCancel = true,
											const QString &cancelText = {});
//! A shortcut to show a busy indicator dialog
Q_MVVMCORE_EXPORT ProgressControl *showBusy(const QString &title = {},
											const QString &label = {},
											bool allowCancel = true,
											const QString &cancelText = {});
//! @}

}

Q_DECLARE_METATYPE(QtMvvm::MessageConfig)
Q_DECLARE_METATYPE(QtMvvm::MessageConfig::StandardButton)
Q_DECLARE_TYPEINFO(QtMvvm::MessageConfig, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(QtMvvm::MessageResult*)
Q_DECLARE_OPERATORS_FOR_FLAGS(QtMvvm::MessageConfig::StandardButtons)

#endif // QTMVVM_MESSAGE_H

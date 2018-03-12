#include <QObject>

//! A QML signelton to access the QtMvvm namespace methods for showing simple dialogs
class Message : public QObject
{
public:
	//! @brief A QML version of the QtMvvm::information method
	//! @copydetails QtMvvm::information(const QString &, const QString &, const std::function<void()> &, const QString &)
	static void information(string title, string text, function onResult, string okText);
	//! @brief A QML version of the QtMvvm::question method
	//! @copydoc QtMvvm::question(const QString &, const QString &, const std::function<void(bool)> &, const QString &, const QString &)
	static void question(string title, string text, function onResult, string yesText, string noText);
	//! @brief A QML version of the QtMvvm::warning method
	//! @copydoc QtMvvm::warning(const QString &, const QString &, const std::function<void()> &, const QString &)
	static void warning(string title, string text, function onResult, string okText);
	//! @brief A QML version of the QtMvvm::critical method
	//! @copydoc QtMvvm::critical(const QString &, const QString &, const std::function<void()> &, const QString &)
	static void critical(string title, string text, function onResult, string okText);
	//! @brief A QML version of the QtMvvm::about method
	//! @copydoc QtMvvm::about
	static void about(string description,
					  url websiteUrl,
					  string licenseName,
					  url licenseUrl,
					  string companyName,
					  bool addQtVersion,
					  list extraTopInfos,
					  string extraBottomInfos);

	//! @brief A QML version of the QtMvvm::getInput method
	//! @copydoc QtMvvm::getInput(const QString &, const QString &, const char *, const std::function<void(QVariant)> &, const QVariant &, const QVariantMap &, const QString &, const QString &)
	static void getInput(string title,
						 string text,
						 string inputType,
						 function onResult,
						 var defaultValue,
						 object viewProperties,
						 string okText,
						 string cancelText);

	//! @brief A QML version of the QtMvvm::getExistingDirectory method
	//! @copydoc QtMvvm::getExistingDirectory(const std::function<void(QUrl)> &, const QString &, const QUrl &)
	static void getExistingDirectory(function onResult, string title, url dir);
	//! @brief A QML version of the QtMvvm::getOpenFile method
	//! @copydoc QtMvvm::getOpenFile(const std::function<void(QUrl)> &, const QString &, const QStringList &, const QUrl &)
	static void getOpenFile(function onResult, string title, list supportedMimeTypes, url dir);
	//! @brief A QML version of the QtMvvm::getOpenFiles method
	//! @copydoc QtMvvm::getOpenFiles(const std::function<void(QList<QUrl>)> &, const QString &, const QStringList &, const QUrl &)
	static void getOpenFiles(function onResult, string title, list supportedMimeTypes, url dir);
	//! @brief A QML version of the QtMvvm::getSaveFile method
	//! @copydoc QtMvvm::getSaveFile(const std::function<void(QUrl)> &, const QString &, const QStringList &, const QUrl &)
	static void getSaveFile(function onResult, string title, list supportedMimeTypes, url dir);
}

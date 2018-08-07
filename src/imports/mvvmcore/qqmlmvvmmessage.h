#ifndef QTMVVM_QQMLMVVMMESSAGE_H
#define QTMVVM_QQMLMVVMMESSAGE_H

#include <QtCore/QObject>

#include <QtQml/QQmlParserStatus>
#include <QtQml/QJSValue>
#include <QtQml/QJSEngine>

#include <QtMvvmCore/Messages>

#ifdef DOXYGEN_RUN
namespace de::skycoder42::QtMvvm::Core {

/*! @brief A QML signelton to access the QtMvvm namespace methods for showing simple dialogs
 *
 * @extends QtQml.QtObject
 * @since 1.0
 *
 * @sa QtMvvm::MessageConfig, QtMvvm::CoreApp::showDialog
 */
class Message
#else
namespace QtMvvm {

class QQmlMvvmMessage : public QObject
#endif
{
	Q_OBJECT

public:
	//! @private
	explicit QQmlMvvmMessage(QJSEngine *engine, QObject *parent = nullptr);

#ifdef DOXYGEN_RUN
public:
#else
public Q_SLOTS:
#define static
#endif
	//! @brief A QML version of the QtMvvm::information method
	//! @copydetails QtMvvm::information(const QString &, const QString &, const std::function<void()> &, const QString &)
	static void information(const QString &title,
							const QString &text,
							const QJSValue &onResult = {},
							const QString &okText = {});
	//! @brief A QML version of the QtMvvm::question method
	//! @copydoc QtMvvm::question(const QString &, const QString &, const std::function<void(bool)> &, const QString &, const QString &)
	static void question(const QString &title,
						 const QString &text,
						 const QJSValue &onResult = {},
						 const QString &yesText = {},
						 const QString &noText = {});
	//! @brief A QML version of the QtMvvm::warning method
	//! @copydoc QtMvvm::warning(const QString &, const QString &, const std::function<void()> &, const QString &)
	static void warning(const QString &title,
						const QString &text,
						const QJSValue &onResult = {},
						const QString &okText = {});
	//! @brief A QML version of the QtMvvm::critical method
	//! @copydoc QtMvvm::critical(const QString &, const QString &, const std::function<void()> &, const QString &)
	static void critical(const QString &title,
						 const QString &text,
						 const QJSValue &onResult = {},
						 const QString &okText = {});

	//! @brief A QML version of the QtMvvm::about method
	//! @copydoc QtMvvm::about
	static void about(const QString &description,
					  const QUrl &websiteUrl = QUrl(),
					  const QString &licenseName = QString(),
					  const QUrl &licenseUrl = QUrl(),
					  const QString &companyName = QString(),
					  bool addQtVersion = true,
					  const QStringList &extraTopInfos = QStringList(),
					  const QString &extraBottomInfos = QString());

	//! @brief A QML version of the QtMvvm::getInput method
	//! @copydoc QtMvvm::getInput(const QString &, const QString &, const char *, const std::function<void(QVariant)> &, const QVariant &, const QVariantMap &, const QString &, const QString &)
	static void getInput(const QString &title,
						 const QString &text,
						 const QString &inputType,
						 const QJSValue &onResult = {},
						 const QVariant &defaultValue = {},
						 const QVariantMap &viewProperties = {},
						 const QString &okText = {},
						 const QString &cancelText = {});

	//! @brief A QML version of the QtMvvm::getExistingDirectory method
	//! @copydoc QtMvvm::getExistingDirectory(const std::function<void(QUrl)> &, const QString &, const QUrl &)
	static void getExistingDirectory(const QJSValue &onResult = {},
									 const QString &title = {},
									 const QUrl &dir = {});
	//! @brief A QML version of the QtMvvm::getOpenFile method
	//! @copydoc QtMvvm::getOpenFile(const std::function<void(QUrl)> &, const QString &, const QStringList &, const QUrl &)
	static void getOpenFile(const QJSValue &onResult = {},
							const QString &title = {},
							const QStringList &supportedMimeTypes = {},
							const QUrl &dir = {});
	//! @brief A QML version of the QtMvvm::getOpenFiles method
	//! @copydoc QtMvvm::getOpenFiles(const std::function<void(QList<QUrl>)> &, const QString &, const QStringList &, const QUrl &)
	static void getOpenFiles(const QJSValue &onResult,
							 const QString &title = {},
							 const QStringList &supportedMimeTypes = {},
							 const QUrl &dir = {});
	//! @brief A QML version of the QtMvvm::getSaveFile method
	//! @copydoc QtMvvm::getSaveFile(const std::function<void(QUrl)> &, const QString &, const QStringList &, const QUrl &)
	static void getSaveFile(const QJSValue &onResult,
							const QString &title = {},
							const QStringList &supportedMimeTypes = {},
							const QUrl &dir = {});

	//! @brief A QML version of the QtMvvm::getColor method
	//! @copydoc QtMvvm::getColor(const std::function<void(QColor)> &, const QString &, const QColor &, bool)
	QTMVVM_REVISION_1 static void getColor(const QJSValue &onResult = {},
										   const QString &title = {},
										   const QColor &color = {},
										   bool argb = false);

	//! @brief A QML version of the QtMvvm::showProgress method
	//! @copydoc QtMvvm::showProgress(const QString &, const QString &, int, int, bool, int, const QString &)
	QTMVVM_REVISION_1 static QtMvvm::ProgressControl *showProgress(const QString &title = {},
																   const QString &label = {},
																   int maximum = 100,
																   int minimum = 0,
																   bool allowCancel = true,
																   int value = 0,
																   const QString &cancelText = {});
	//! @brief A QML version of the QtMvvm::showIndeterminateProgress method
	//! @copydoc QtMvvm::showIndeterminateProgress(const QString &, const QString &, bool, const QString &)
	QTMVVM_REVISION_1 static QtMvvm::ProgressControl *showIndeterminateProgress(const QString &title = {},
																				const QString &label = {},
																				bool allowCancel = true,
																				const QString &cancelText = {});
	//! @brief A QML version of the QtMvvm::showBusy method
	//! @copydoc QtMvvm::showBusy(const QString &, const QString &, bool, const QString &)
	QTMVVM_REVISION_1 static QtMvvm::ProgressControl *showBusy(const QString &title = {},
															   const QString &label = {},
															   bool allowCancel = true,
															   const QString &cancelText = {});

#ifndef DOXYGEN_RUN
#undef static
#endif

private:
	QJSEngine *_engine;
};

}

#endif // QTMVVM_QQMLMVVMMESSAGE_H

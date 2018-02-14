#ifndef QTMVVM_QQMLMVVMMESSAGE_H
#define QTMVVM_QQMLMVVMMESSAGE_H

#include <QtCore/QObject>

#include <QtQml/QQmlParserStatus>
#include <QtQml/QJSValue>
#include <QtQml/QJSEngine>

#include <QtMvvmCore/Messages>

namespace QtMvvm {

class QQmlMvvmMessage : public QObject
{
	Q_OBJECT

public:
	explicit QQmlMvvmMessage(QJSEngine *engine, QObject *parent = nullptr);

public Q_SLOTS:
	void information(const QString &title,
					 const QString &text,
					 const QJSValue &onResult = {},
					 const QString &okText = {});
	void question(const QString &title,
				  const QString &text,
				  const QJSValue &onResult = {},
				  const QString &yesText = {},
				  const QString &noText = {});
	void warning(const QString &title,
				 const QString &text,
				 const QJSValue &onResult = {},
				 const QString &okText = {});
	void critical(const QString &title,
				  const QString &text,
				  const QJSValue &onResult = {},
				  const QString &okText = {});

	void about(const QString &description,
			   const QUrl &websiteUrl = QUrl(),
			   const QString &licenseName = QString(),
			   const QUrl &licenseUrl = QUrl(),
			   const QString &companyName = QString(),
			   bool addQtVersion = true,
			   const QStringList &extraTopInfos = QStringList(),
			   const QString &extraBottomInfos = QString());

	void getInput(const QString &title,
				  const QString &text,
				  const char *inputType,
				  const QJSValue &onResult = {},
				  const QVariant &defaultValue = {},
				  const QVariantMap &viewProperties = {},
				  const QString &okText = {},
				  const QString &cancelText = {});

	void getExistingDirectory(const QJSValue &onResult = {},
							  const QString &title = {},
							  const QUrl &dir = {});
	void getOpenFile(const QJSValue &onResult = {},
					 const QString &title = {},
					 const QStringList &supportedMimeTypes = {},
					 const QUrl &dir = {});

	void getOpenFiles(const QJSValue &onResult,
					  const QString &title = {},
					  const QStringList &supportedMimeTypes = {},
					  const QUrl &dir = {});
	void getSaveFile(const QJSValue &onResult,
					 const QString &title = {},
					 const QStringList &supportedMimeTypes = {},
					 const QUrl &dir = {});

private:
	QJSEngine *_engine;
};

}

#endif // QTMVVM_QQMLMVVMMESSAGE_H

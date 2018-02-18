#include "qtmvvmcore_plugin.h"

#include <QtQml>

#include <QtMvvmCore/ViewModel>
#include <QtMvvmCore/Messages>

#include "qqmlmvvmbinding.h"
#include "qqmlmvvmmessage.h"

static QObject *createMessageSingleton(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
	return new QtMvvm::QQmlMvvmMessage(jsEngine, qmlEngine);
}

QtMvvmCoreDeclarativeModule::QtMvvmCoreDeclarativeModule(QObject *parent) :
	QQmlExtensionPlugin(parent)
{}

void QtMvvmCoreDeclarativeModule::registerTypes(const char *uri)
{
	Q_ASSERT(qstrcmp(uri, "de.skycoder42.QtMvvm.Core") == 0);

	//Version 1.0
	qmlRegisterUncreatableType<QtMvvm::MessageConfig>(uri, 1, 0, "MessageConfig", QStringLiteral("Q_GADGETS cannot be created from QML"));
	qmlRegisterUncreatableType<QtMvvm::MessageResult>(uri, 1, 0, "MessageResult", QStringLiteral("Message results must be passed from C++"));
	qmlRegisterUncreatableType<QtMvvm::ViewModel>(uri, 1, 0, "ViewModel", QStringLiteral("ViewModels cannot be created from QML"));

	qmlRegisterType<QtMvvm::QQmlMvvmBinding>(uri, 1, 0, "MvvmBinding");

	qmlRegisterSingletonType<QtMvvm::QQmlMvvmMessage>(uri, 1, 0, "Message", createMessageSingleton);

	// Check to make shure no module update is forgotten
	static_assert(VERSION_MAJOR == 1 && VERSION_MINOR == 0, "QML module version needs to be updated");
}

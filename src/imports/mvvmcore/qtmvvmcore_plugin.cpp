#include "qtmvvmcore_plugin.h"

#include <QtQml>

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
	Q_ASSERT(qstrcmp(uri, "de.skycoder42.qtmvvm.core") == 0);

	qmlRegisterType<QtMvvm::QQmlMvvmBinding>(uri, 1, 0, "Binding");
	qmlRegisterSingletonType<QtMvvm::QQmlMvvmMessage>(uri, 1, 0, "Message", createMessageSingleton);
}

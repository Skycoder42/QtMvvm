#include "qtmvvmcore_plugin.h"

#include <QtQml>

#include <QtMvvmCore/ViewModel>

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
	qmlRegisterUncreatableType<QtMvvm::ViewModel>(uri, 1, 0, "ViewModel", tr("ViewModels cannot be created from QML"));
	qmlRegisterType<QtMvvm::QQmlMvvmBinding>(uri, 1, 0, "MvvmBinding");
	qmlRegisterSingletonType<QtMvvm::QQmlMvvmMessage>(uri, 1, 0, "Message", createMessageSingleton);

	// Check to make shure no module update is forgotten
	static_assert(VERSION_MAJOR == 1 && VERSION_MINOR == 0, "QML module version needs to be updated");
}

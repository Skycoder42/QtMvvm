#include "qtmvvmdatasyncquick_plugin.h"

#include <QtQml>

#include "testdummy.h"

static void initResources()
{
#ifdef QT_STATIC
	Q_INIT_RESOURCE(qtmvvmdatasyncquick_plugin);
#endif
}

QtMvvmDataSyncQuickDeclarativeModule::QtMvvmDataSyncQuickDeclarativeModule(QObject *parent) :
	QQmlExtensionPlugin(parent)
{
	initResources();
}

void QtMvvmDataSyncQuickDeclarativeModule::registerTypes(const char *uri)
{
	Q_ASSERT(qstrcmp(uri, "de.skycoder42.QtMvvm.DataSync.Quick") == 0);

	//Version 1.0
	qmlRegisterType<TestDummy>(uri, 1, 0, "TestDummy");

	// Check to make shure no module update is forgotten
	static_assert(VERSION_MAJOR == 1 && VERSION_MINOR == 0, "QML module version needs to be updated");
}

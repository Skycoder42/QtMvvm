#include "qtmvvmdatasyncquick_plugin.h"

#include <QtQml>

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
	//no c++ types. plugin is only needed for the resources
	qmlRegisterModule(uri, 1, 0);

	//Version 1.1
	qmlRegisterModule(uri, 1, 1);

	// Check to make sure no module update is forgotten
	static_assert(VERSION_MAJOR == 1 && VERSION_MINOR == 1, "QML module version needs to be updated");
}

#ifndef QTMVVMDATASYNCCORE_PLUGIN_H
#define QTMVVMDATASYNCCORE_PLUGIN_H

#include <QtQml/QQmlExtensionPlugin>

class QtMvvmDataSyncCoreDeclarativeModule : public QQmlExtensionPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
	QtMvvmDataSyncCoreDeclarativeModule(QObject *parent = nullptr);
	void registerTypes(const char *uri) override;
};

#endif // QTMVVMDATASYNCCORE_PLUGIN_H

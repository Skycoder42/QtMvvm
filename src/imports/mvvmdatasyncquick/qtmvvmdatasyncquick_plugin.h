#ifndef QTMVVMDATASYNCQUICK_PLUGIN_H
#define QTMVVMDATASYNCQUICK_PLUGIN_H

#include <QtQml/QQmlExtensionPlugin>

class QtMvvmDataSyncQuickDeclarativeModule : public QQmlExtensionPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
	QtMvvmDataSyncQuickDeclarativeModule(QObject *parent = nullptr);
	void registerTypes(const char *uri) override;
};

#endif // QTMVVMDATASYNCQUICK_PLUGIN_H

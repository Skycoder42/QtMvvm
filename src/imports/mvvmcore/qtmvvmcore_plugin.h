#ifndef QTMVVMCORE_PLUGIN_H
#define QTMVVMCORE_PLUGIN_H

#include <QtQml/QQmlExtensionPlugin>

class QtMvvmCoreDeclarativeModule : public QQmlExtensionPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
	QtMvvmCoreDeclarativeModule(QObject *parent = nullptr);
	void registerTypes(const char *uri) override;
};

#endif // QTMVVMCORE_PLUGIN_H

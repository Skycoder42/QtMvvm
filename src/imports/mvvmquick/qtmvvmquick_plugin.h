#ifndef QTMVVMQUICK_PLUGIN_H
#define QTMVVMQUICK_PLUGIN_H

#include <QtQml/QQmlExtensionPlugin>

class QtMvvmQuickDeclarativeModule : public QQmlExtensionPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
	QtMvvmQuickDeclarativeModule(QObject *parent = nullptr);
	void registerTypes(const char *uri) override;
};

#endif // QTMVVMQUICK_PLUGIN_H

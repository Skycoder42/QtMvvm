#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include "../serviceregistry/plugintestinterface.h"

class TestPlugin : public QObject, public PluginTestInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginTestInterface)
	Q_PLUGIN_METADATA(IID PluginTestInterfaceIId FILE "serviceregistrytestplugin.json")

public:
	TestPlugin(QObject *parent = nullptr);

	int magicNumber() const override;
};

#endif // TESTPLUGIN_H

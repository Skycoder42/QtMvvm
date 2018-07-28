#ifndef PLUGINTESTINTERFACE_H
#define PLUGINTESTINTERFACE_H

#include <QtCore/QObject>

class PluginTestInterface
{
	Q_DISABLE_COPY(PluginTestInterface)
public:
	inline PluginTestInterface() = default;
	virtual inline ~PluginTestInterface() = default;

	virtual int magicNumber() const = 0;
};

#define PluginTestInterfaceIId "de.skycoder42.qtmvvm.core.tests.PluginTestInterface"
Q_DECLARE_INTERFACE(PluginTestInterface, PluginTestInterfaceIId)

#endif // PLUGINTESTINTERFACE_H

#ifndef TESTINTERFACE_H
#define TESTINTERFACE_H

#include <QtCore/QObject>

class TestInterface
{
	Q_DISABLE_COPY(TestInterface)
public:
	inline TestInterface() = default;
	virtual inline ~TestInterface() = default;
};

#define TestInterfaceIId "de.skycoder42.qtmvvm.core.tests.TestInterface"
Q_DECLARE_INTERFACE(TestInterface, TestInterfaceIId)

#endif // TESTINTERFACE_H

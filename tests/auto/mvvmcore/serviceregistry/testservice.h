#ifndef TESTSERVICE_H
#define TESTSERVICE_H

#include <QtCore/QObject>
#include "testinterface.h"

class TestService : public QObject, public TestInterface
{
	Q_OBJECT
	Q_INTERFACES(TestInterface)

public:
	Q_INVOKABLE explicit TestService(QObject *parent = nullptr);
};

class WeakTestService : public TestService
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit WeakTestService(QObject *parent = nullptr);
};

#endif // TESTSERVICE_H

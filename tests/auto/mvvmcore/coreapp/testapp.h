#ifndef TESTAPP_H
#define TESTAPP_H

#include <QtMvvmCore/CoreApp>
#include "testpresenter.h"

class TestApp : public QtMvvm::CoreApp
{
	Q_OBJECT

public:
	explicit TestApp(QObject *parent = nullptr);

	bool wasRegistered = false;
	QStringList startArgs;
	bool wasClosed = false;

	static TestPresenter *presenter();

protected:
	void performRegistrations() override;
	int startApp(const QStringList &arguments) override;
	void closeApp() override;
};

#undef coreApp
#define coreApp static_cast<TestApp*>(QtMvvm::CoreApp::instance())

#endif // TESTAPP_H

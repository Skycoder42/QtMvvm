#include "testapp.h"
#include <QtMvvmCore/ServiceRegistry>

TestApp::TestApp(QObject *parent) :
	CoreApp(parent)
{}

TestPresenter *TestApp::presenter()
{
	return static_cast<TestPresenter*>(CoreApp::presenter());
}

void TestApp::performRegistrations()
{
	wasRegistered = true;
}

int TestApp::startApp(const QStringList &arguments)
{
	startArgs = arguments;
	return EXIT_SUCCESS;
}

void TestApp::closeApp()
{
	wasClosed = false;
}

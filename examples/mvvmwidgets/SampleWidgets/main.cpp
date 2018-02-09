#include "mainwindow.h"
#include <QApplication>

#include <QtMvvmCore/ServiceRegistry>

#include "widgetseventservice.h"

#define TEST_DIRECT 0
#define TEST_FN 1
#define TEST_INST 2
#define TEST_CURRENT TEST_DIRECT

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	if(TEST_CURRENT == TEST_DIRECT)
			QtMvvm::ServiceRegistry::instance()->registerObject<EchoService>();
	if(TEST_CURRENT == TEST_FN)
		QtMvvm::ServiceRegistry::instance()->registerObject<EchoService>([]() {
			return new EchoService(nullptr);
		});
	if(TEST_CURRENT == TEST_INST)
		QtMvvm::ServiceRegistry::instance()->registerObject<EchoService>(new EchoService());

	if(TEST_CURRENT == TEST_DIRECT)
		QtMvvm::ServiceRegistry::instance()->registerInterface<IEventService, WidgetsEventService>();
	if(TEST_CURRENT == TEST_FN)
		QtMvvm::ServiceRegistry::instance()->registerInterface<IEventService, WidgetsEventService>([](EchoService *echo) {
			return new WidgetsEventService(echo, nullptr);
		});
	if(TEST_CURRENT == TEST_INST)
		QtMvvm::ServiceRegistry::instance()->registerInterface<IEventService>(new WidgetsEventService(QtMvvm::ServiceRegistry::instance()->acquireInstance<EchoService>()));

	//debug test
	auto event = QtMvvm::ServiceRegistry::instance()->acquireInstance<IEventService>();

	return a.exec();
}

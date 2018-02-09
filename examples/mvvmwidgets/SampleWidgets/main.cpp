#include "mainwindow.h"
#include <QApplication>

#include <QtMvvmCore/ServiceRegistry>

#include "widgetseventservice.h"

struct test : public QObject {
	IEventService *i;
	EchoService *s;

	test(IEventService *i, EchoService *s) :
		i(i),
		s(s)
	{}
};

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QtMvvm::ServiceRegistry::instance()->registerObject<EchoService>();
	if (false) {
		QtMvvm::ServiceRegistry::instance()->registerObject<EchoService>([]() {
			return new EchoService(nullptr);
		});
		QtMvvm::ServiceRegistry::instance()->registerObject<EchoService>(new EchoService());
	}

	QtMvvm::ServiceRegistry::instance()->registerInterface<IEventService, WidgetsEventService>();
	if (false) {
		QtMvvm::ServiceRegistry::instance()->registerInterface<IEventService, WidgetsEventService>([](EchoService *echo) {
			return new WidgetsEventService(echo, nullptr);
		});
		QtMvvm::ServiceRegistry::instance()->registerInterface<IEventService>(new WidgetsEventService());
	}

	return a.exec();
}

#include <QtWidgets/QApplication>
#include <QtMvvmCore/ServiceRegistry>
#include <QtMvvmWidgets/WidgetsPresenter>
#include <samplecoreapp.h>

#include "widgetseventservice.h"
#include "sampleview.h"
#include "resultdialog.h"
#include "tabview.h"
#include "tabitemview.h"

#define TEST_DIRECT 0
#define TEST_FN 1
#define TEST_INST 2
#define TEST_CURRENT TEST_DIRECT

QTMVVM_REGISTER_CORE_APP(SampleCoreApp)

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QtMvvm::WidgetsPresenter::registerView<SampleView>();
	QtMvvm::WidgetsPresenter::registerView<ResultDialog>();
	QtMvvm::WidgetsPresenter::registerView<TabView>();
	QtMvvm::WidgetsPresenter::registerView<TabItemView>();

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
		QtMvvm::ServiceRegistry::instance()->registerInterface<IEventService>(new WidgetsEventService(QtMvvm::ServiceRegistry::instance()->service<EchoService>()));

	return a.exec();
}

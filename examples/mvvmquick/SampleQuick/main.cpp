#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtMvvmCore/ServiceRegistry>
#include <QtMvvmQuick/QuickPresenter>

#include <samplecoreapp.h>
#include <echoservice.h>

#include "quickeventservice.h"

QTMVVM_REGISTER_CORE_APP(SampleCoreApp)

int main(int argc, char *argv[])
{
#ifdef QML_PATH
	qputenv("QML2_IMPORT_PATH", QML_PATH);
#endif

	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QGuiApplication app(argc, argv);

	QtMvvm::QuickPresenter::registerAsPresenter();

	QtMvvm::ServiceRegistry::instance()->registerObject<EchoService>();
	QtMvvm::ServiceRegistry::instance()->registerInterface<IEventService, QuickEventService>();

	QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;

	return app.exec();
}

#include <QtWidgets/QApplication>
#include <QQmlApplicationEngine>
#include <QtMvvmCore/ServiceRegistry>
#include <QtMvvmQuick/QuickPresenter>
#include <QtQuickControls2/QQuickStyle>
#include <QtCore/QDebug>

#include <quickextras.h>

#include <samplecoreapp.h>
#include <echoservice.h>

#include <sampleviewmodel.h>
#include <resultviewmodel.h>
#include <drawerviewmodel.h>
#include <tabviewmodel.h>

#include "quickeventservice.h"

QTMVVM_REGISTER_CORE_APP(SampleCoreApp)

int main(int argc, char *argv[])
{
#ifdef QML_PATH
	qputenv("QML2_IMPORT_PATH", QML_PATH);
#endif

	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication app(argc, argv);
	qDebug() << QQuickStyle::availableStyles() << QQuickStyle::name();

	coreApp->setShowDrawer(true);

	qmlRegisterUncreatableType<SampleViewModel>("de.skycoder42.QtMvvm.Sample", 1, 0, "SampleViewModel", QStringLiteral("ViewModels cannot be created"));
	qmlRegisterUncreatableType<ResultViewModel>("de.skycoder42.QtMvvm.Sample", 1, 0, "ResultViewModel", QStringLiteral("ViewModels cannot be created"));
	qmlRegisterUncreatableType<DrawerViewModel>("de.skycoder42.QtMvvm.Sample", 1, 0, "DrawerViewModel", QStringLiteral("ViewModels cannot be created"));
	qmlRegisterUncreatableType<TabViewModel>("de.skycoder42.QtMvvm.Sample", 1, 0, "TabViewModel", QStringLiteral("ViewModels cannot be created"));
	qmlRegisterUncreatableType<TabItemViewModel>("de.skycoder42.QtMvvm.Sample", 1, 0, "TabItemViewModel", QStringLiteral("ViewModels cannot be created"));

	QtMvvm::QuickPresenter::registerAsPresenter();

	QtMvvm::ServiceRegistry::instance()->registerObject<EchoService>();
	QtMvvm::ServiceRegistry::instance()->registerInterface<IEventService, QuickEventService>();

	QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;

	return app.exec();
}

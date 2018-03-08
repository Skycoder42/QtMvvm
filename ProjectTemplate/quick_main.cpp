#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <quickpresenter.h>
#include <%{AppHdrName}>

REGISTER_CORE_APP(%{AppCn})

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QGuiApplication app(argc, argv);

	qmlRegisterUncreatableType<%{ControlCn}>("com.example.%{ProjectLowerName}", 1, 0, "%{ControlCn}", "Controls cannot be created!");

	QuickPresenter::createAppEngine(QUrl(QLatin1String("qrc:/qml/App.qml")));

	return app.exec();
}

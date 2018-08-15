#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <%{AppHdrName}>
#include <%{VmHdrName}>

QTMVVM_REGISTER_CORE_APP(%{AppCn})

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	// If you want to support file dialogs on platforms other then android, use a QApplication instead (and add QT += widgets to the pro file)
	QGuiApplication app(argc, argv);
	
	QtMvvm::QuickPresenter::getInputViewFactory(); //Workaround for QTBUG-69963
	qmlRegisterUncreatableType<%{VmCn}>("com.example.%{ProjectLowerName}", 1, 0, "%{VmCn}", "ViewModels cannot be created!");
	
	QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/App.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;

	return app.exec();
}

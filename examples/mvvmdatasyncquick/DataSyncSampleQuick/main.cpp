#include <QtWidgets/QApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtMvvmDataSyncQuick/qtmvvmdatasyncquick_global.h>

#include <samplecoreapp.h>
#include <sampleviewmodel.h>

QTMVVM_REGISTER_CORE_APP(SampleCoreApp)

int main(int argc, char *argv[])
{
#ifdef QML_PATH
	qputenv("QML2_IMPORT_PATH", QML_PATH);
#endif

	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication app(argc, argv);

	QtMvvm::registerDataSyncQuick();
	qmlRegisterUncreatableType<SampleViewModel>("de.skycoder42.QtMvvm.Sample", 1, 1, "SampleViewModel", QStringLiteral("ViewModels cannot be created"));

	QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;

	return app.exec();
}

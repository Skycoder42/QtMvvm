#include "qtmvvmquick_plugin.h"

#include <QtQml>

#include "qqmlquickpresenter.h"
#include "svgimageprovider.h"

static void initResources()
{
#ifdef QT_STATIC
	Q_INIT_RESOURCE(qtmvvmquick_plugin);
#endif
}

static QObject *createQuickPresenterQmlSingleton(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
	Q_UNUSED(jsEngine)
	//image provider is created together with the singleton
	qmlEngine->addImageProvider(QStringLiteral("svg"), new QtMvvm::SvgImageProvider());
	return new QtMvvm::QQmlQuickPresenter(qmlEngine);
}

QtMvvmQuickDeclarativeModule::QtMvvmQuickDeclarativeModule(QObject *parent) :
	QQmlExtensionPlugin(parent)
{
	initResources();
}

void QtMvvmQuickDeclarativeModule::registerTypes(const char *uri)
{
	Q_ASSERT(qstrcmp(uri, "de.skycoder42.QtMvvm.Quick") == 0);

	//Version 1.0
	qmlRegisterSingletonType<QtMvvm::QQmlQuickPresenter>(uri, 1, 0, "QuickPresenter", createQuickPresenterQmlSingleton);

	// Check to make shure no module update is forgotten
	static_assert(VERSION_MAJOR == 1 && VERSION_MINOR == 0, "QML module version needs to be updated");
}

#include "qtmvvmquick_plugin.h"

#include <QtQml>

#include <QtMvvmQuick/InputViewFactory>

#include "qqmlquickpresenter.h"
#include "settingsuibuilder.h"
#include "qqmlviewplaceholder.h"
#ifdef Q_OS_ANDROID
#include "androidfilechooser.h"
#endif

static void initResources()
{
#ifdef QT_STATIC
	Q_INIT_RESOURCE(qtmvvmquick_plugin);
#endif
}

static QObject *createQuickPresenterQmlSingleton(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
	Q_UNUSED(jsEngine)
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
	qmlRegisterUncreatableType<QtMvvm::InputViewFactory>(uri, 1, 0, "InputViewFactory", QStringLiteral("InputViewFactories can only be created from C++ via the QuickPresenter"));
	qmlRegisterSingletonType<QtMvvm::QQmlQuickPresenter>(uri, 1, 0, "QuickPresenter", createQuickPresenterQmlSingleton);

	qmlRegisterType<QtMvvm::SettingsUiBuilder>(uri, 1, 0, "SettingsUiBuilder");

#ifdef Q_OS_ANDROID
	qmlRegisterType<QtMvvm::AndroidFileChooser>(uri, 1, 0, "FileChooser");
	qmlRegisterType(QUrl(QStringLiteral("qrc:/de/skycoder42/qtmvvm/quick/qml/AndroidFileDialog.qml")), uri, 1, 0, "FileDialog");
	qmlRegisterType(QUrl(QStringLiteral("qrc:/de/skycoder42/qtmvvm/quick/qml/AndroidFolderDialog.qml")), uri, 1, 0, "FolderDialog");
#else
	qmlRegisterType(QUrl(QStringLiteral("qrc:/de/skycoder42/qtmvvm/quick/qml/FileDialog.qml")), uri, 1, 0, "FileDialog");
	qmlRegisterType(QUrl(QStringLiteral("qrc:/de/skycoder42/qtmvvm/quick/qml/FolderDialog.qml")), uri, 1, 0, "FolderDialog");
#endif

	//Version 1.1
	qmlRegisterType<QtMvvm::QQmlViewPlaceholder>(uri, 1, 1, "ViewPlaceholder");

	// Check to make sure no module update is forgotten
	static_assert(VERSION_MAJOR == 1 && VERSION_MINOR == 1, "QML module version needs to be updated");
}

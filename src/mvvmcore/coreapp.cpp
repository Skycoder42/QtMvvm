#include "coreapp.h"
#include "coreapp_p.h"

#include <QtCore/QCommandLineParser>

#include <QtGui/QGuiApplication>

using namespace QtMvvm;

CoreApp::CoreApp(QObject *parent) :
	QObject(parent),
	d(new CoreAppPrivate())
{}

CoreApp::~CoreApp() {}

CoreApp *CoreApp::instance()
{
	return CoreAppPrivate::instance;
}

void CoreApp::disableAutoBoot()
{
	CoreAppPrivate::bootEnabled = false;
}

void CoreApp::registerApp()
{
	//register metatypes
	setParent(qApp);
	CoreAppPrivate::instance = this;
	performRegistrations();
	if(CoreAppPrivate::bootEnabled)
		QMetaObject::invokeMethod(this, "bootApp", Qt::QueuedConnection);
}

void CoreApp::bootApp()
{
	auto res = startApp(QCoreApplication::arguments());
	if(res == EXIT_SUCCESS) {
		connect(qApp, &QCoreApplication::aboutToQuit,
				this, &CoreApp::closeApp);
	} else
		qApp->exit(res);
}

void CoreApp::performRegistrations() {}

void CoreApp::closeApp() {}

bool CoreApp::autoParse(QCommandLineParser &parser, const QStringList &arguments)
{
	if(parser.parse(arguments)) {
		if(parser.isSet(QStringLiteral("help"))) {
			Q_UNIMPLEMENTED();
			return false;
		} else if(parser.isSet(QStringLiteral("version"))) {
			auto text = QGuiApplication::applicationDisplayName() +
						QLatin1Char(' ') +
						QCoreApplication::applicationVersion();
			Q_UNIMPLEMENTED();
			return false;
		} else
			return true;
	} else {
		Q_UNIMPLEMENTED();
		return false;
	}
}

// ------------- Private Implementation -------------

bool CoreAppPrivate::bootEnabled = true;
QPointer<CoreApp> CoreAppPrivate::instance = nullptr;

CoreAppPrivate::CoreAppPrivate()
{

}

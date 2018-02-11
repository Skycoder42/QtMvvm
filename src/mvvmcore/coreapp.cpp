#include "coreapp.h"
#include "coreapp_p.h"
#include "qtmvvm_logging_p.h"
#include "serviceregistry_p.h"

#include <QtCore/QCommandLineParser>
#include <QtCore/QRegularExpression>

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

void CoreApp::setMainPresenter(IPresenter *presenter)
{
	if(!CoreAppPrivate::instance)
		logCritical() << "Failed to set presenter - no core app has been created yet";
	else
		CoreAppPrivate::instance->d->presenter.reset(presenter);
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
	if(!d->presenter)
		logWarning() << "No presenter has been set before the app start";
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

CoreAppPrivate::CoreAppPrivate() :
	presenter(nullptr)
{}

QScopedPointer<CoreAppPrivate> &CoreAppPrivate::dInstance()
{
	return instance->d;
}

void CoreAppPrivate::showViewModel(const QMetaObject *metaObject, const QVariantHash &params, QPointer<ViewModel> parent, quint32 requestCode)
{
	if(presenter) {
		QPointer<ViewModel> vm;
		try {
			auto obj = ServiceRegistryPrivate::constructInjected(metaObject);
			vm = qobject_cast<ViewModel*>(obj);
			if(!vm)
				throw ServiceConstructionException("Invalid types - not at QtMvvm::ViewModel");
			presenter->present(vm, params, parent);
			if(requestCode != 0) {
				QObject::connect(vm, &ViewModel::resultReady,
								 parent, &ViewModel::onResult,
								 Qt::UniqueConnection);
			}
		} catch(QException &e) {
			logCritical() << "Failed to present viewmodel of type"
						  << metaObject->className()
						  << "with error:"
						  << e.what();
			if(vm)
				vm->deleteLater();
		}
	} else {
		logCritical() << "Failed to present viewmodel of type"
					  << metaObject->className()
					  << "- no presenter was set";
	}
}

IPresenter *CoreAppPrivate::currentPresenter() const
{
	return presenter.data();
}

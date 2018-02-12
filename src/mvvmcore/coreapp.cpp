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
	qRegisterMetaType<const QMetaObject*>("const QMetaObject*");

	//setup
	setParent(qApp);
	CoreAppPrivate::instance = this;
	performRegistrations();
	if(CoreAppPrivate::bootEnabled)
		QMetaObject::invokeMethod(this, "bootApp", Qt::QueuedConnection);
}

IPresenter *CoreApp::presenter() const
{
	return d->presenter.data();
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

void CoreApp::show(const char *viewModelName, const QVariantHash &params) const
{
	auto metaId = QMetaType::type(viewModelName);
	auto metaObject = QMetaType::metaObjectForType(metaId);
	if(!metaObject) {
		throw PresenterException(QByteArrayLiteral("Given name (") +
								 viewModelName +
								 QByteArrayLiteral(") does not name a type with meta data"));
	}
	show(metaObject, params);
}

void CoreApp::show(const QMetaObject *viewMetaObject, const QVariantHash &params) const
{
	if(!viewMetaObject->inherits(&ViewModel::staticMetaObject)) {
		throw PresenterException(QByteArrayLiteral("Given type (") +
								 viewMetaObject->className() +
								 QByteArrayLiteral(") is not a class that extends QtMvvm::ViewModel"));
	}
	ViewModel::showImp(viewMetaObject, params, nullptr);
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
	QMetaObject::invokeMethod(this, "showViewModelPrivate", Qt::QueuedConnection,
							  Q_ARG(const QMetaObject*, metaObject),
							  Q_ARG(const QVariantHash&, params),
							  Q_ARG(QPointer<ViewModel>, parent),
							  Q_ARG(quint32, requestCode));
}

IPresenter *CoreAppPrivate::currentPresenter() const
{
	return presenter.data();
}

void CoreAppPrivate::showViewModelPrivate(const QMetaObject *metaObject, const QVariantHash &params, QPointer<ViewModel> parent, quint32 requestCode)
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
				QObject::connect(vm, &ViewModel::resultReady, parent, [vm, requestCode, parent](const QVariant &r){
					vm->disconnect(parent);
					parent->onResult(requestCode, r);
				});
				QObject::connect(vm, &ViewModel::destroyed, parent, [vm, requestCode, parent](){
					parent->onResult(requestCode, QVariant());
				});
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

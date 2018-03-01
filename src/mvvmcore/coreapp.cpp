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

void CoreApp::disableAutoBoot()
{
	CoreAppPrivate::bootEnabled = false;
}

void CoreApp::registerApp()
{
	//register metatypes
	qRegisterMetaType<const QMetaObject*>("const QMetaObject*");
	qRegisterMetaType<MessageConfig::StandardButton>();
	registerInterfaceConverter<IPresenter>();

	//setup
	setParent(qApp);
	CoreAppPrivate::instance = this;
	performRegistrations();
	if(CoreAppPrivate::bootEnabled)
		QMetaObject::invokeMethod(this, "bootApp", Qt::QueuedConnection);
}

void CoreApp::show(const char *viewModelName, const QVariantHash &params)
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

void CoreApp::show(const QMetaObject *viewMetaObject, const QVariantHash &params)
{
	if(!viewMetaObject->inherits(&ViewModel::staticMetaObject)) {
		throw PresenterException(QByteArrayLiteral("Given type (") +
								 viewMetaObject->className() +
								 QByteArrayLiteral(") is not a class that extends QtMvvm::ViewModel"));
	}
	showImp(viewMetaObject, params);
}

MessageResult *CoreApp::showDialog(const MessageConfig &config)
{
	auto result = new MessageResult();
	QMetaObject::invokeMethod(CoreAppPrivate::dInstance().data(), "showDialog", Qt::QueuedConnection,
							  Q_ARG(QtMvvm::MessageConfig, config),
							  Q_ARG(QtMvvm::MessageResult*, result));
	return result;
}

void CoreApp::bootApp()
{
	if(!d->presenter) {
		try {
			d->presenter = ServiceRegistry::instance()->service<IPresenter>();
		} catch(ServiceConstructionException &e) {
			logCritical() << "Unable to get a presenter with error"
						  << e.what();
			qApp->exit(EXIT_FAILURE);
			return;
		}
	}

	auto res = startApp(QCoreApplication::arguments());
	if(res == EXIT_SUCCESS) {
		connect(qApp, &QCoreApplication::aboutToQuit,
				this, &CoreApp::closeApp);
		emit appStarted();
	} else
		qApp->exit(res);
}

void CoreApp::performRegistrations() {}

void CoreApp::closeApp() {}

bool CoreApp::autoParse(QCommandLineParser &parser, const QStringList &arguments)
{
	if(parser.parse(arguments)) {
		if(parser.isSet(QStringLiteral("help"))) {
			MessageConfig helpConfig {MessageConfig::TypeMessageBox, MessageConfig::SubTypeQuestion};
			helpConfig.setTitle(tr("Help"))
					.setText(parser.helpText())
					.setButtons(MessageConfig::Ok);
			auto res = showDialog(helpConfig);
			connect(res, &MessageResult::dialogDone,
					qApp, &QCoreApplication::quit);
			return false;
		} else if(parser.isSet(QStringLiteral("version"))) {
			MessageConfig versionConfig {MessageConfig::TypeMessageBox, MessageConfig::SubTypeInformation};
			versionConfig.setTitle(tr("Application Version"))
					.setText(QGuiApplication::applicationDisplayName() +
							 QLatin1Char(' ') +
							 QCoreApplication::applicationVersion())
					.setButtons(MessageConfig::Ok);
			auto res = showDialog(versionConfig);
			connect(res, &MessageResult::dialogDone,
					qApp, &QCoreApplication::quit);
			return false;
		} else
			return true;
	} else {
		MessageConfig errorConfig {MessageConfig::TypeMessageBox, MessageConfig::SubTypeCritical};
		errorConfig.setTitle(tr("Invalid Arguments"))
				.setText(parser.errorText())
				.setButtons(MessageConfig::Ok);
		auto res = showDialog(errorConfig);
		connect(res, &MessageResult::dialogDone,
				qApp, [](){
			qApp->exit(EXIT_FAILURE);
		});
		return false;
	}
}

void CoreApp::showImp(const QMetaObject *metaObject, const QVariantHash &params)
{
	QMetaObject::invokeMethod(CoreAppPrivate::dInstance().data(), "showViewModel", Qt::QueuedConnection,
							  Q_ARG(const QMetaObject*, metaObject),
							  Q_ARG(QVariantHash, params),
							  Q_ARG(QPointer<ViewModel>, nullptr),
							  Q_ARG(quint32, 0));
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

void CoreAppPrivate::showDialog(const MessageConfig &config, MessageResult *result)
{
	if(presenter) {
		try {
			presenter->showDialog(config, result);
		} catch(QException &e) {
			logCritical() << "Failed to show dialog ff type"
						  << config.type() << ":" << config.subType()
						  << "with error:"
						  << e.what();
			result->complete(MessageConfig::NoButton);
		}
	}
	else {
		logCritical() << "Failed to show dialog ff type"
					  << config.type() << ":" << config.subType()
					  << "- no presenter was set";
		result->complete(MessageConfig::NoButton);
	}
}

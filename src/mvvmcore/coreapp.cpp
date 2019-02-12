#include "coreapp.h"
#include "coreapp_p.h"
#include "qtmvvm_logging_p.h"
#include "serviceregistry_p.h"
#include "settingssetup.h"

#include <QtCore/QCommandLineParser>
#include <QtCore/QRegularExpression>
#include <QtCore/QMetaClassInfo>

#include <QtGui/QGuiApplication>

using namespace QtMvvm;

CoreApp::CoreApp(QObject *parent) :
	QObject(parent),
	d(new CoreAppPrivate())
{}

CoreApp::~CoreApp() = default;

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

	qRegisterMetaType<ViewModel*>();
	qRegisterMetaType<SettingsElements::Entry>();
	qRegisterMetaType<SettingsElements::Group>();
	qRegisterMetaType<SettingsElements::Section>();
	qRegisterMetaType<SettingsElements::Category>();
	qRegisterMetaType<SettingsElements::Setup>();

	registerInterfaceConverter<IPresenter>();

	//setup
	setParent(qApp);
	CoreAppPrivate::instance = this;
	performRegistrations();

	//self-inject properties (after performRegistrations, as they might register an interface converter)
	ServiceRegistry::instance()->injectServices(this);

	if(CoreAppPrivate::bootEnabled)
		QMetaObject::invokeMethod(this, "bootApp", Qt::QueuedConnection);
}

void CoreApp::show(const char *viewModelName, const QVariantHash &params)
{
	show(viewModelName, params, nullptr);
}

void CoreApp::show(const char *viewModelName, const QVariantHash &params, QPointer<ViewModel> parentViewModel)
{
	auto metaId = QMetaType::type(viewModelName);
	auto metaObject = QMetaType::metaObjectForType(metaId);
	if(!metaObject) {
		throw PresenterException(QByteArrayLiteral("Given name (") +
								 viewModelName +
								 QByteArrayLiteral(") does not name a type with meta data"));
	}
	show(metaObject, params, std::move(parentViewModel));
}

void CoreApp::show(const QMetaObject *viewModelMetaObject, const QVariantHash &params)
{
	show(viewModelMetaObject, params, nullptr);
}

void CoreApp::show(const QMetaObject *viewModelMetaObject, const QVariantHash &params, QPointer<ViewModel> parentViewModel)
{
	if(!viewModelMetaObject->inherits(&ViewModel::staticMetaObject)) {
		throw PresenterException(QByteArrayLiteral("Given type (") +
								 viewModelMetaObject->className() +
								 QByteArrayLiteral(") is not a class that extends QtMvvm::ViewModel"));
	}
	showImp(viewModelMetaObject, params, std::move(parentViewModel));
}

MessageResult *CoreApp::showDialog(const MessageConfig &config)
{
	auto result = new MessageResult();
	QMetaObject::invokeMethod(CoreAppPrivate::dInstance().data(), "showDialog", Qt::QueuedConnection,
							  Q_ARG(QtMvvm::MessageConfig, config),
							  Q_ARG(QtMvvm::MessageResult*, result));
	return result;
}

QVariant CoreApp::safeCastInputType(const QByteArray &type, const QVariant &value)
{
	// get the target type, either explicitly or from the name
	auto mType = CoreAppPrivate::dInstance()->inputTypeMapping.value(type, QMetaType::UnknownType);
	if(mType == QMetaType::UnknownType) {
		mType = QMetaType::type(type.constData());
		if(mType == QMetaType::UnknownType) { // no type found -> do nothing but warn
			logWarning() << "Input type" << type << "has no know C++ type to convert it to. Use QtMvvm::CoreApp::registerInputTypeMapping to add it."
						 << "To surpress this warning without performing a conversion, register it as `CoreApp::registerInputTypeMapping<QVariant>(" << type << ");`";
			return value;
		}
	}

	// target type is QVariant -> return without converting
	if(mType == QMetaType::QVariant)
		return value;

	auto convValue = value;
	if(!convValue.convert(mType)) {
		logWarning() << "Failed to convert data for input type" << type
					 << "from given type" << value.typeName()
					 << "to the target type" << QMetaType::typeName(mType);
		return value;
	} else
		return convValue;
}

void CoreApp::registerInputTypeMapping(const QByteArray &type, int targetType)
{
	CoreAppPrivate::dInstance()->inputTypeMapping.insert(type, targetType);
}

IPresenter *CoreApp::presenter()
{
	return CoreAppPrivate::dInstance()->presenter;
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
		emit appStarted({});
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
	showImp(metaObject, params, nullptr);
}

void CoreApp::showImp(const QMetaObject *metaObject, const QVariantHash &params, QPointer<ViewModel> parentViewModel)
{
	QMetaObject::invokeMethod(CoreAppPrivate::dInstance().data(), "showViewModel", Qt::QueuedConnection,
							  Q_ARG(const QMetaObject*, metaObject),
							  Q_ARG(QVariantHash, params),
							  Q_ARG(QPointer<ViewModel>, parentViewModel),
							  Q_ARG(quint32, 0));
}

// ------------- Private Implementation -------------

bool CoreAppPrivate::bootEnabled = true;
QPointer<CoreApp> CoreAppPrivate::instance = nullptr;

CoreAppPrivate::CoreAppPrivate() :
	inputTypeMapping{
		{"switch", QMetaType::Bool},
		{"string", QMetaType::QString},
		{"number", QMetaType::Double},
		{"range", QMetaType::Int},
		{"date", QMetaType::QDateTime},
		{"color", QMetaType::QColor},
		{"url", QMetaType::QUrl},
		{"var", QMetaType::QVariant},
		{"variant", QMetaType::QVariant},
		{"selection", QMetaType::QVariant},
		{"list", QMetaType::QVariant},
		{"radiolist", QMetaType::QVariant},
	}
{}

QScopedPointer<CoreAppPrivate> &CoreAppPrivate::dInstance()
{
	return instance->d;
}

void CoreAppPrivate::showViewModel(const QMetaObject *metaObject, const QVariantHash &params, QPointer<ViewModel> parent, quint32 requestCode)
{
	showViewModelWithReturn(metaObject, params, std::move(parent), requestCode);
}

void CoreAppPrivate::showDialog(const MessageConfig &config, MessageResult *result)
{
	if(presenter) {
		try {
			presenter->showDialog(config, result);
			logDebug() << "Successfully presented dialog of type" << config.type();
		} catch(QTMVVM_EXCEPTION_BASE &e) {
			logCritical() << "Failed to show dialog for type"
						  << config.type() << ":" << config.subType()
						  << "with error:"
						  << e.what();
			result->complete(MessageConfig::NoButton);
		}
	} else {
		logCritical() << "Failed to show dialog ff type"
					  << config.type() << ":" << config.subType()
					  << "- no presenter was set";
		result->complete(MessageConfig::NoButton);
	}
}

bool CoreAppPrivate::isSingleton(const QMetaObject *metaObject) const
{
	auto sInfoIndex = metaObject->indexOfClassInfo("qtmvvm_singleton");
	if(sInfoIndex != -1) {
		auto sInfo = metaObject->classInfo(sInfoIndex);
		Q_ASSERT(qstrcmp(sInfo.name(), "qtmvvm_singleton") == 0);
		return qstrcmp(sInfo.value(), "true") == 0;
	} else
		return false;
}

const QMetaObject *CoreAppPrivate::getContainer(const QMetaObject *metaObject) const
{
	auto cInfoIndex = metaObject->indexOfClassInfo("qtmvvm_container_viewmodel");
	if(cInfoIndex != -1) {
		auto cInfo = metaObject->classInfo(cInfoIndex);
		Q_ASSERT(qstrcmp(cInfo.name(), "qtmvvm_container_viewmodel") == 0);
		auto typeId = QMetaType::type(QByteArray{cInfo.value() + QByteArray{"*"}}.constData());
		if(typeId == QMetaType::UnknownType) {
			throw PresenterException {
				QByteArrayLiteral("Unabled to find the qtmvvm_container_viewmodel of type \"") + cInfo.value() +
				QByteArrayLiteral("\" for viewmodel of type \"") + metaObject->className() +
				QByteArrayLiteral("\" - Make sure to register the container via qRegisterMetaType<") + cInfo.value() +
				QByteArrayLiteral("*>()")
			};
		}
		auto containerMo = QMetaType::metaObjectForType(typeId);
		if(!containerMo) {
			throw PresenterException {
				QByteArrayLiteral("The qtmvvm_container_viewmodel of type \"") + cInfo.value() +
				QByteArrayLiteral("\" for viewmodel of type \"") + metaObject->className() +
				QByteArrayLiteral("\" does exists, but does not have a QMetaObject")
			};
		}
		if(!containerMo->inherits(&ViewModel::staticMetaObject)) {
			throw PresenterException {
				QByteArrayLiteral("The qtmvvm_container_viewmodel of type \"") + cInfo.value() +
				QByteArrayLiteral("\" for viewmodel of type \"") + metaObject->className() +
				QByteArrayLiteral("\" does exists, but does not extend QtMvvm::ViewModel")
			};
		}
		return containerMo;
	} else
		return nullptr;
}

QPointer<ViewModel> CoreAppPrivate::showViewModelWithReturn(const QMetaObject *metaObject, const QVariantHash &params, QPointer<ViewModel> parent, quint32 requestCode)
{
	if(presenter) {
		// first: handle a singleton
		auto isSingle = isSingleton(metaObject);
		if(isSingle) {
			auto viewModel = singleInstances.value(metaObject);
			if(viewModel) {
				logDebug() << "Found existing single instance for" << metaObject->className();
				emit viewModel->instanceInvoked(params, ViewModel::QPrivateSignal{});
				return viewModel;
			}
		}

		// next: check if it has a container, and if yes: show the container
		try {
			auto container = getContainer(metaObject);
			if(container) {
				logDebug() << "Found container type for" << metaObject->className()
						   << "as" << container->className();
				auto containerParams = params.value(QStringLiteral("qtmvvm_container_params")).toHash();
				containerParams.insert(QStringLiteral("qtmvvm_container_for"), QByteArray{metaObject->className()});
				parent = showViewModelWithReturn(container, containerParams, std::move(parent), 0);
				if(!parent)
					throw PresenterException{"Failed to present parent container"};
			}
		} catch(PresenterException &e) {
			logCritical() << "Failed to present viewmodel of type"
						  << metaObject->className()
						  << "with error:"
						  << e.what();
			return nullptr;
		}

		QPointer<ViewModel> vm;
		try {
			auto obj = ServiceRegistry::instance()->constructInjected(metaObject);
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
			logDebug() << "Successfully presented" << metaObject->className();

			// if singleton -> store it
			if(isSingle)
				singleInstances.insert(metaObject, vm);
			return vm;
		} catch(QTMVVM_EXCEPTION_BASE &e) {
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

	return nullptr;
}

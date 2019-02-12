#include "serviceregistry.h"
#include "serviceregistry_p.h"
#include "qtmvvm_logging_p.h"

#include <QtCore/QGlobalStatic>
#include <QtCore/QRegularExpression>
#include <QtCore/QMetaProperty>
#include <QtCore/QCoreApplication>
#include <QtCore/QThread>

#include <qpluginfactory.h>

using namespace QtMvvm;

Q_GLOBAL_STATIC_WITH_ARGS(ServiceRegistry, _instance,
						  (new ServiceRegistryPrivate()))

ServiceRegistry::ServiceRegistry(ServiceRegistryPrivate *d_ptr) :
	d(d_ptr)
{
	// register quit scope cleanup
	QObject::connect(qApp, &QCoreApplication::aboutToQuit, [this](){
		d->destroyServices(DestroyOnAppQuit);
	});
	// register destroy scope cleanup
	qAddPostRoutine(&ServiceRegistryPrivate::appDestroyedHook);
}

ServiceRegistry::~ServiceRegistry()
{
	d->destroyServices(DestroyOnRegistryDestroy);
}

ServiceRegistry *ServiceRegistry::instance()
{
	return _instance;
}

bool ServiceRegistry::isRegistered(const QByteArray &iid) const
{
	QMutexLocker _(&d->serviceMutex);
	return d->services.contains(iid);
}

void ServiceRegistry::registerService(const QByteArray &iid, const QMetaObject *metaObject, DestructionScope scope, bool weak)
{
	QMutexLocker _(&d->serviceMutex);
	if(d->serviceBlocked(iid))
		throw ServiceExistsException(iid);
	d->services.insert(iid, QSharedPointer<ServiceRegistryPrivate::MetaServiceInfo>::create(metaObject, weak, scope));
}

void ServiceRegistry::registerService(const QByteArray &iid, const QMetaObject *metaObject, bool weak)
{
	registerService(iid, metaObject, DestroyOnAppDestroy, weak);
}

void ServiceRegistry::registerService(const QByteArray &iid, const std::function<QObject *(const QObjectList &)> &fn, QByteArrayList injectables, DestructionScope scope, bool weak)
{
	QMutexLocker _(&d->serviceMutex);
	if(d->serviceBlocked(iid))
		throw ServiceExistsException(iid);
	d->services.insert(iid, QSharedPointer<ServiceRegistryPrivate::FnServiceInfo>::create(fn, std::move(injectables), weak, scope));
}

void ServiceRegistry::registerService(const QByteArray &iid, const std::function<QObject*(const QObjectList &)> &fn, QByteArrayList injectables, bool weak)
{
	registerService(iid, fn, std::move(injectables), DestroyOnAppDestroy, weak);
}

void ServiceRegistry::registerService(QByteArray iid, QString pluginType, QString pluginKey, ServiceRegistry::DestructionScope scope, bool weak)
{
	QMutexLocker _(&d->serviceMutex);
	if(d->serviceBlocked(iid))
		throw ServiceExistsException(iid);
	auto info = QSharedPointer<ServiceRegistryPrivate::PluginServiceInfo>::create(std::move(pluginType),
																				  std::move(pluginKey),
																				  std::move(iid),
																				  weak,
																				  scope);
	d->services.insert(info->iid(), info);
}

QObject *ServiceRegistry::serviceObj(const QByteArray &iid)
{
	QMutexLocker _(&d->serviceMutex);
	auto ref = d->services.value(iid);
	if(ref)
		return ref->instance(d.data(), iid);
	else
		throw ServiceDependencyException(iid);
}

void ServiceRegistry::injectServices(QObject *object)
{
	QMutexLocker _(&d->serviceMutex);
	d->injectLocked(object);
}

QObject *ServiceRegistry::constructInjected(const QMetaObject *metaObject, QObject *parent)
{
	QMutexLocker _(&d->serviceMutex);
	return d->constructInjectedLocked(metaObject, parent);
}

// ------------- Private Implementation -------------

bool ServiceRegistryPrivate::serviceBlocked(const QByteArray &iid) const
{
	auto svc = services.value(iid);
	if(svc)
		return !svc->replaceable();
	else
		return false;
}

QObject *ServiceRegistryPrivate::constructInjectedLocked(const QMetaObject *metaObject, QObject *parent)
{
	auto instance = metaObject->newInstance(Q_ARG(QObject*, parent));
	if(!instance) {
		throw ServiceConstructionException(QByteArrayLiteral("Failed to construct object of type ") +
										   metaObject->className() +
										   QByteArrayLiteral(" - make sure there is an invokable constructor of the format: Q_INVOKABLE MyClass(QObject*)"));
	}

	try {
		injectLocked(instance);
		return instance;
	} catch (...) {
		instance->deleteLater();
		throw;
	}
}

void ServiceRegistryPrivate::injectLocked(QObject *object)
{
	static QRegularExpression nameRegex(QStringLiteral(R"__(^__qtmvvm_inject_(.+)$)__"));

	auto metaObject = object->metaObject();
	for(auto i = 0; i < metaObject->propertyCount(); i++) {
		auto prop = metaObject->property(i);
		auto match = nameRegex.match(QString::fromUtf8(prop.name()));
		if(match.hasMatch()) {
			auto tPropIndex = metaObject->indexOfProperty(qUtf8Printable(match.captured(1)));
			if(tPropIndex == -1) {
				logWarning().noquote() << "Found hint to inject property"
									   << match.captured(1)
									   << "but no property of that name was found";
				continue;
			}
			auto tProp = metaObject->property(tPropIndex);

			auto iid = prop.read(object).toByteArray();
			auto ref = services.value(iid);
			if(!ref)
				throw ServiceDependencyException(iid);
			auto injObj = ref->instance(this, iid);
			auto variant = QVariant::fromValue(injObj);
			if(!variant.convert(tProp.userType())) {
				throw ServiceConstructionException("Failed to convert QObject to interface with iid \"" +
												   iid +
												   "\". Use QtMvvm::registerInterfaceConverter to make it convertable "
												   "or change the property's type to \"QObject*\"");
			}
			tProp.write(object, variant);
		}
	}

	// if available, call qtmvvm_init
	auto initMethod = metaObject->indexOfMethod("qtmvvm_init()");
	if(initMethod != -1) {
		auto method = metaObject->method(initMethod);
		method.invoke(object);
	}
}

void ServiceRegistryPrivate::destroyServices(ServiceRegistry::DestructionScope scope)
{
	QMutexLocker _(&serviceMutex);
	logDebug() << "Beginning destruction for services in scope:" << scope;
	for(auto it = services.begin(); it != services.end();) {
		if(it.value()->needsDestroy(scope)) {
			logDebug() << "Destroying service:" << it.key();
			it = services.erase(it);
		} else
			it++;
	}
	logDebug() << "Finished destruction for services in scope:" << scope;
}

void ServiceRegistryPrivate::appDestroyedHook()
{
	if(_instance.exists() && !_instance.isDestroyed())
		_instance->d->destroyServices(ServiceRegistry::DestroyOnAppDestroy);
}



ServiceRegistryPrivate::ServiceInfo::ServiceInfo(bool weak, ServiceRegistry::DestructionScope scope) :
	_weak{weak},
	_scope{scope}
{}

ServiceRegistryPrivate::ServiceInfo::~ServiceInfo()
{
	if(_instance && _scope != ServiceRegistry::DestroyNever) {
		if(_closingDown)
			delete _instance;
		else
			QMetaObject::invokeMethod(_instance, "deleteLater");
	}
}

bool ServiceRegistryPrivate::ServiceInfo::replaceable() const
{
	return _weak && !_instance;
}

bool ServiceRegistryPrivate::ServiceInfo::needsDestroy(ServiceRegistry::DestructionScope scope) const
{
	if(_scope <= scope) {
		_closingDown = true;
		return true;
	} else
		return false;
}

QObject *ServiceRegistryPrivate::ServiceInfo::instance(ServiceRegistryPrivate *d, const QByteArray &iid)
{
	if(!_instance) {
		_instance = construct(d);
		if(!_instance)
			throw ServiceConstructionException("Failed to construct service of type " +
											   iid +
											   " with unknown error");
		if(_instance->thread() != qApp->thread())
			_instance->moveToThread(qApp->thread());
		logDebug() << "Constructed service of type" << iid;
	}
	return _instance;
}



ServiceRegistryPrivate::FnServiceInfo::FnServiceInfo(std::function<QObject*(QObjectList)> &&creator, QByteArrayList &&injectables, bool weak, ServiceRegistry::DestructionScope scope) :
	ServiceInfo{weak, scope},
	_creator{std::move(creator)},
	_injectables{std::move(injectables)}
{}

QObject *ServiceRegistryPrivate::FnServiceInfo::construct(ServiceRegistryPrivate *d) const
{
	QObjectList params;
	params.reserve(_injectables.size());
	for(const auto &iid : _injectables) {
		auto ref = d->services.value(iid);
		if(!ref)
			throw ServiceDependencyException(iid);
		params.append(ref->instance(d, iid));
	}
	return _creator(params);
}



ServiceRegistryPrivate::MetaServiceInfo::MetaServiceInfo(const QMetaObject *metaObject, bool weak, ServiceRegistry::DestructionScope scope) :
	ServiceInfo{weak, scope},
	_metaObject{metaObject}
{}

QObject *ServiceRegistryPrivate::MetaServiceInfo::construct(ServiceRegistryPrivate *d) const
{
	return d->constructInjectedLocked(_metaObject, nullptr); //services are created without a parent
}



ServiceRegistryPrivate::PluginServiceInfo::PluginServiceInfo(QString &&type, QString &&key, QByteArray &&iid, bool weak, ServiceRegistry::DestructionScope scope) :
	ServiceInfo{weak, scope},
	_type{std::move(type)},
	_key{std::move(key)},
	_iid{std::move(iid)}
{}

const QByteArray &ServiceRegistryPrivate::PluginServiceInfo::iid() const
{
	return _iid;
}

QObject *ServiceRegistryPrivate::PluginServiceInfo::construct(ServiceRegistryPrivate *d) const
{
	try {
		QFileInfo typeInfo{_type};
		QPluginFactoryBase factory{typeInfo.isAbsolute() ? QString{} : _type, _iid};
		if(typeInfo.isAbsolute()) {
			factory.addSearchDir(typeInfo.absolutePath(), false);
			factory.reloadPlugins();
		}

		QObject *obj = nullptr;
		if(_key.isEmpty()){
			if(!factory.allKeys().isEmpty())
				obj = factory.plugin(factory.allKeys().value(0));
		} else
			obj = factory.plugin(_key);
		if(!obj) {
			throw ServiceConstructionException{"No plugin of type \"" + _type.toUtf8() +
											   "\" found that can provide the interface \"" + _iid +
											   (_key.isEmpty() ? QByteArray{"\""} : QByteArray{"\" for the key: " + _key.toUtf8()})};
		}
		d->injectLocked(obj);
		return obj;
	} catch(QPluginLoadException &e) { //convert exception...
		throw ServiceConstructionException{e.what()};
	}
}

// ------------- Exceptions Implementation -------------

ServiceExistsException::ServiceExistsException(const QByteArray &iid) :
	_msg("An interface with the type id \"" + iid + "\" has already been registered")
{}

ServiceExistsException::ServiceExistsException(const ServiceExistsException * const other) :
	_msg(other->_msg)
{}

const char *ServiceExistsException::what() const noexcept
{
	return _msg.constData();
}

void ServiceExistsException::raise() const
{
	throw (*this);
}

QTMVVM_EXCEPTION_BASE *ServiceExistsException::clone() const
{
	return new ServiceExistsException(this);
}



ServiceConstructionException::ServiceConstructionException(const QByteArray &message) :
	_msg(message)
{}

ServiceConstructionException::ServiceConstructionException(const ServiceConstructionException * const other) :
	_msg(other->_msg)
{}

const char *ServiceConstructionException::what() const noexcept
{
	return _msg.constData();
}

void ServiceConstructionException::raise() const
{
	throw (*this);
}

QTMVVM_EXCEPTION_BASE *ServiceConstructionException::clone() const
{
	return new ServiceConstructionException(this);
}



ServiceDependencyException::ServiceDependencyException(const QByteArray &iid) :
	ServiceConstructionException("Failed to construct missing service: " + iid)
{}

ServiceDependencyException::ServiceDependencyException(const ServiceDependencyException * const other) :
	ServiceConstructionException(other)
{}

void ServiceDependencyException::raise() const
{
	throw (*this);
}

QTMVVM_EXCEPTION_BASE *ServiceDependencyException::clone() const
{
	return new ServiceDependencyException(this);
}

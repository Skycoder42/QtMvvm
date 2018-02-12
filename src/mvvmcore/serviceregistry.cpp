#include "serviceregistry.h"
#include "serviceregistry_p.h"
#include "qtmvvm_logging_p.h"

#include <QtCore/QGlobalStatic>
#include <QtCore/QRegularExpression>
#include <QtCore/QMetaProperty>

using namespace QtMvvm;

Q_GLOBAL_STATIC_WITH_ARGS(ServiceRegistry, _instance,
						  (new ServiceRegistryPrivate()))

ServiceRegistry::ServiceRegistry(ServiceRegistryPrivate *d_ptr) :
	d(d_ptr)
{}

ServiceRegistry *ServiceRegistry::instance()
{
	return _instance;
}

void ServiceRegistry::registerService(const QByteArray &iid, const QMetaObject *metaObject)
{
	QMutexLocker _(&d->serviceMutex);
	if(d->services.contains(iid))
		throw ServiceExistsException(iid);
	d->services.insert(iid, QSharedPointer<ServiceRegistryPrivate::MetaServiceInfo>::create(metaObject));
}

void ServiceRegistry::registerService(const QByteArray &iid, const std::function<QObject*(const QObjectList &)> &fn, QByteArrayList injectables)
{
	QMutexLocker _(&d->serviceMutex);
	if(d->services.contains(iid))
		throw ServiceExistsException(iid);
	d->services.insert(iid, QSharedPointer<ServiceRegistryPrivate::FnServiceInfo>::create(fn, injectables));
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

// ------------- Private Implementation -------------

ServiceRegistryPrivate::ServiceRegistryPrivate() :
	serviceMutex(),
	services()
{}

QObject *ServiceRegistryPrivate::constructInjected(const QMetaObject *metaObject)
{
	auto &d = ServiceRegistry::instance()->d;
	QMutexLocker _(&d->serviceMutex);
	return d->constructInjectedLocked(metaObject);
}

QObject *ServiceRegistryPrivate::constructInjectedLocked(const QMetaObject *metaObject)
{
	auto instance = metaObject->newInstance();
	if(!instance) {
		throw ServiceConstructionException(QByteArrayLiteral("Failed to construct object of type ") +
										   metaObject->className() +
										   QByteArrayLiteral(" - make shure there is an invokable constructor of the format: Q_INVOKABLE MyClass(QObject*)"));
	}

	static QRegularExpression nameRegex(QStringLiteral(R"__(^__qtmvvm_inject_(.+)$)__"),
										QRegularExpression::OptimizeOnFirstUsageOption);

	try {
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

				auto iid = prop.read(instance).toByteArray();
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
				tProp.write(instance, variant);
			}
		}

		return instance;
	} catch (...) {
		instance->deleteLater();
		throw;
	}
}



ServiceRegistryPrivate::ServiceInfo::ServiceInfo() :
	_instance(nullptr)
{}

ServiceRegistryPrivate::ServiceInfo::~ServiceInfo()
{
	if(_instance)
		QMetaObject::invokeMethod(_instance, "deleteLater");
}

QObject *ServiceRegistryPrivate::ServiceInfo::instance(ServiceRegistryPrivate *d, const QByteArray &iid)
{
	if(!_instance) {
		logDebug() << "Constructing service of type" << iid;
		_instance = construct(d);
		if(!_instance)
			throw ServiceConstructionException("Failed to construct service of type " +
											   iid +
											   " with unknown error");
	}
	return _instance;
}



ServiceRegistryPrivate::FnServiceInfo::FnServiceInfo(const std::function<QObject*(QObjectList)> &creator, const QByteArrayList &injectables) :
	creator(creator),
	injectables(injectables)
{}

QObject *ServiceRegistryPrivate::FnServiceInfo::construct(ServiceRegistryPrivate *d) const
{
	QObjectList params;
	for(auto iid : injectables) {
		auto ref = d->services.value(iid);
		if(!ref)
			throw ServiceDependencyException(iid);
		params.append(ref->instance(d, iid));
	}
	return creator(params);
}



ServiceRegistryPrivate::MetaServiceInfo::MetaServiceInfo(const QMetaObject *metaObject) :
	metaObject(metaObject)
{}

QObject *ServiceRegistryPrivate::MetaServiceInfo::construct(ServiceRegistryPrivate *d) const
{
	auto instance = d->constructInjectedLocked(metaObject);
	auto initMethod = metaObject->indexOfMethod("qtmvvm_init()"); //TODO document
	if(initMethod != -1) {
		auto method = metaObject->method(initMethod);
		method.invoke(instance);
	}
	return instance;
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

QException *ServiceExistsException::clone() const
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

QException *ServiceConstructionException::clone() const
{
	return new ServiceConstructionException(this);
}



ServiceDependencyException::ServiceDependencyException(const QByteArray &iid) :
	ServiceConstructionException("Failed to construct service because of missing dependency: " + iid)
{}

ServiceDependencyException::ServiceDependencyException(const ServiceDependencyException * const other) :
	ServiceConstructionException(other)
{}

void ServiceDependencyException::raise() const
{
	throw (*this);
}

QException *ServiceDependencyException::clone() const
{
	return new ServiceDependencyException(this);
}

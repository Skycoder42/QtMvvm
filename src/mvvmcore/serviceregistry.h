#ifndef QTMVVM_SERVICEREGISTRY_H
#define QTMVVM_SERVICEREGISTRY_H

#include <functional>

#include <QtCore/qscopedpointer.h>
#include <QtCore/qvariant.h>
#include <QtCore/qexception.h>

#include "QtMvvmCore/qtmvvmcore_global.h"
#include "QtMvvmCore/injection.h"

namespace QtMvvm {

class ServiceRegistryPrivate;
//! A singleton to prepare services for dependency injection and to access them
class Q_MVVMCORE_EXPORT ServiceRegistry
{
public:
	//! @private
	ServiceRegistry(ServiceRegistryPrivate *d_ptr);
	~ServiceRegistry();

	//! Returns the ServiceRegistry singleton instance
	static ServiceRegistry* instance();

	//! Checks if a given interface or service is already registered
	template <typename TInterface>
	bool isRegistered() const;
	//! @copybrief ServiceRegistry::isRegistered()
	bool isRegistered(const QByteArray &iid) const;

	//! Register a service for its interface via the type
	template <typename TInterface, typename TService>
	void registerInterface(bool weak = false);
	//! Register a service for its interface via a constructor function
	template <typename TInterface, typename TService, typename TFunc>
	void registerInterface(TFunc fn, bool weak = false);
	//! Register a service for its interface via an already existing instance
	template <typename TInterface, typename TService>
	void registerInterface(TService *service, bool weak = false);
	//! Register a service via its type
	template <typename TService>
	void registerObject(bool weak = false);
	//! Register a service via a constructor function
	template <typename TService, typename TFunc>
	void registerObject(TFunc fn, bool weak = false);
	//! Register a service via an already existing instance
	template <typename TService>
	void registerObject(TService *service, bool weak = false);

	//! Register a service by an iid via their metadata
	void registerService(const QByteArray &iid,
						 const QMetaObject *metaObject,
						 bool weak = false);
	//! Register a service by an iid via a generalized constructor function
	void registerService(const QByteArray &iid,
						 const std::function<QObject*(const QObjectList &)> &fn,
						 QByteArrayList injectables,
						 bool weak = false);

	//! Returns the service for the given interface
	template <typename TInterface>
	TInterface *service();
	//! Returns the service for the given iid
	QObject *serviceObj(const QByteArray &iid);

	//! Inject services for all injectable properties on object
	void injectServices(QObject *object);
	//! Constructs a new instance of TClass with properties injected
	template <typename TClass>
	TClass *constructInjected(QObject *parent = nullptr);
	//! Constructs a new instance of metaObject with properties injected
	QObject *constructInjected(const QMetaObject *metaObject, QObject *parent = nullptr);

private:
	friend class QtMvvm::ServiceRegistryPrivate;
	QScopedPointer<ServiceRegistryPrivate> d;
};

//! Is thrown if a service is beeing registered that is already registered
class Q_MVVMCORE_EXPORT ServiceExistsException : public QException
{
public:
	//! @private
	ServiceExistsException(const QByteArray &iid);

	//! @inherit{QException::what}
	const char *what() const noexcept override;

	//! @inherit{QException::raise}
	void raise() const override;
	//! @inherit{QException::clone}
	QException *clone() const override;

protected:
	//! @private
	ServiceExistsException(const ServiceExistsException * const other);

	//! @private
	const QByteArray _msg;
};

//! Is thrown in case the construction of a service has failed
class Q_MVVMCORE_EXPORT ServiceConstructionException : public QException
{
public:
	//! @private
	ServiceConstructionException(const QByteArray &message);

	//! @inherit{QException::what}
	const char *what() const noexcept override;

	//! @inherit{QException::raise}
	void raise() const override;
	//! @inherit{QException::clone}
	QException *clone() const override;

protected:
	//! @private
	ServiceConstructionException(const ServiceConstructionException * const other);

	//! @private
	const QByteArray _msg;
};

//! Is thrown if a service could not be created because of a missing dependency to be injected
class Q_MVVMCORE_EXPORT ServiceDependencyException : public ServiceConstructionException
{
public:
	//! @private
	ServiceDependencyException(const QByteArray &iid);

	void raise() const override;
	QException *clone() const override;

protected:
	//! @private
	ServiceDependencyException(const ServiceDependencyException * const other);
};

// ------------- Generic Implementation -------------

template<typename TInterface>
bool ServiceRegistry::isRegistered() const
{
	return isRegistered(__helpertypes::inject_iid<TInterface*>());
}

#define QTMVVM_SERVICE_ASSERT(tint, tsvc) \
	static_assert(__helpertypes::is_valid_interface<TInterface, TService>::value, "TService must implement the given TInterface interface and be a qobject class"); \
	Q_ASSERT_X(qobject_interface_iid<TInterface*>(), Q_FUNC_INFO, "TInterface must be registered with Q_DECLARE_INTERFACE");

template<typename TInterface, typename TService>
void ServiceRegistry::registerInterface(bool weak)
{
	QTMVVM_SERVICE_ASSERT(TInterface, TService)
	registerService(qobject_interface_iid<TInterface*>(), &TService::staticMetaObject, weak);
}

template <typename TInterface, typename TService, typename TFunc>
void ServiceRegistry::registerInterface(TFunc fn, bool weak)
{
	QTMVVM_SERVICE_ASSERT(TInterface, TService)
	QByteArrayList injectables;
	auto packed_fn = __helpertypes::pack_function(std::move(fn), injectables);
	registerService(qobject_interface_iid<TInterface*>(), packed_fn, injectables, weak);
}

template<typename TInterface, typename TService>
void ServiceRegistry::registerInterface(TService *service, bool weak)
{
	QTMVVM_SERVICE_ASSERT(TInterface, TService)
	registerService(qobject_interface_iid<TInterface*>(), [service](const QObjectList &params) -> QObject* {
		Q_UNUSED(params);
		return service;
	}, QByteArrayList(), weak);
}

#undef QTMVVM_SERVICE_ASSERT
#define QTMVVM_SERVICE_ASSERT(tsvc) \
	static_assert(__helpertypes::is_qobj<tsvc>::value, "TService must be a qobject class");

template<typename TService>
void ServiceRegistry::registerObject(bool weak)
{
	QTMVVM_SERVICE_ASSERT(TService)
	registerService(__helpertypes::qobject_iid<TService*>(), &TService::staticMetaObject, weak);
}

template<typename TService, typename TFunc>
void ServiceRegistry::registerObject(TFunc fn, bool weak)
{
	QTMVVM_SERVICE_ASSERT(TService)
	QByteArrayList injectables;
	auto packed_fn = __helpertypes::pack_function(std::move(fn), injectables);
	registerService(__helpertypes::qobject_iid<TService*>(), packed_fn, injectables, weak);
}

template<typename TService>
void ServiceRegistry::registerObject(TService *service, bool weak)
{
	QTMVVM_SERVICE_ASSERT(TService)
	registerService(__helpertypes::qobject_iid<TService*>(), [service](const QObjectList &params) -> QObject* {
		Q_UNUSED(params);
		return service;
	}, QByteArrayList(), weak);
}

#undef QTMVVM_SERVICE_ASSERT

template<typename TInterface>
TInterface *ServiceRegistry::service()
{
	return qobject_cast<TInterface*>(serviceObj(__helpertypes::inject_iid<TInterface*>()));
}

template<typename TClass>
TClass *ServiceRegistry::constructInjected(QObject *parent)
{
	static_assert(__helpertypes::is_qobj<TClass>::value, "TClass must be a qobject class");
	return qobject_cast<TClass*>(constructInjected(&TClass::staticMetaObject, parent));
}

}

#endif // QTMVVM_SERVICEREGISTRY_H

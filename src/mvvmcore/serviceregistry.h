#ifndef QTMVVM_SERVICEREGISTRY_H
#define QTMVVM_SERVICEREGISTRY_H

#include <functional>

#include <QtCore/qscopedpointer.h>
#include <QtCore/qvariant.h>

#include "QtMvvmCore/qtmvvmcore_global.h"
#include "QtMvvmCore/injection.h"
#include "QtMvvmCore/exception.h"

namespace QtMvvm {

class ServiceRegistryPrivate;
//! A singleton to prepare services for dependency injection and to access them
class Q_MVVMCORE_EXPORT ServiceRegistry //MAJOR make a QObject for invokable methods and Q_ENUM
{
public:
	//! A scope to indicate when a service should be deleted
	enum DestructionScope {
		DestroyOnAppQuit = 1, //!< Destroy when the QCoreApplication::aboutToQuit signal is emitted
		DestroyOnAppDestroy = 2, //!< Destroy as soon as the QCoreApplication gets destroyed (typically end of main)
		DestroyOnRegistryDestroy = 3, //!< Destroy together with the service registry instance (static memory, unspecified order!)

		DestroyNever = 127 //!< Never destroy the instance
	};

	//! @private
	ServiceRegistry(ServiceRegistryPrivate *d_ptr);
	~ServiceRegistry();

	//! Returns the ServiceRegistry singleton instance
	static ServiceRegistry* instance();

	//! Checks if a given interface or service is already registered
	template <typename TInterface>
	bool isRegistered() const;
	//! @copybrief ServiceRegistry::isRegistered() const
	bool isRegistered(const QByteArray &iid) const;

	//! Register a service for its interface via the type
	template <typename TInterface, typename TService>
	void registerInterface(DestructionScope scope = DestroyOnAppDestroy, bool weak = false);
	//! Register a service for its interface via a constructor function
	template <typename TInterface, typename TService, typename TFunc>
	void registerInterface(TFunc fn, DestructionScope scope = DestroyOnAppDestroy, bool weak = false);
	//! Register a service for its interface via an already existing instance
	template <typename TInterface, typename TService>
	void registerInterface(TService *service, DestructionScope scope = DestroyOnAppDestroy, bool weak = false);

	//! Register a service via its type
	template <typename TService>
	void registerObject(DestructionScope scope = DestroyOnAppDestroy, bool weak = false);
	//! Register a service via a constructor function
	template <typename TService, typename TFunc>
	void registerObject(TFunc fn, DestructionScope scope = DestroyOnAppDestroy, bool weak = false);
	//! Register a service via an already existing instance
	template <typename TService>
	void registerObject(TService *service, DestructionScope scope = DestroyOnAppDestroy, bool weak = false);

	//! Register a service via specifing a plugin to be loaded
	template <typename TInterface>
	void registerPlugin(QString pluginType = {}, QString pluginKey = {}, DestructionScope scope = DestroyOnAppDestroy, bool weak = false);

	//! Register a service by an iid via their metadata
	void registerService(const QByteArray &iid,
						 const QMetaObject *metaObject,
						 DestructionScope scope = DestroyOnAppDestroy,
						 bool weak = false);
	Q_DECL_DEPRECATED void registerService(const QByteArray &iid,
										   const QMetaObject *metaObject,
										   bool weak);
	//! Register a service by an iid via a generalized constructor function
	void registerService(const QByteArray &iid,
						 const std::function<QObject*(const QObjectList &)> &fn,
						 QByteArrayList injectables,
						 DestructionScope scope = DestroyOnAppDestroy,
						 bool weak = false);
	Q_DECL_DEPRECATED void registerService(const QByteArray &iid,
										   const std::function<QObject*(const QObjectList &)> &fn,
										   QByteArrayList injectables,
										   bool weak);
	//! Register a service by an iid via specifing a plugin to be loaded
	void registerService(QByteArray iid,
						 QString pluginType = {},
						 QString pluginKey = {},
						 DestructionScope scope = DestroyOnAppDestroy,
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
class Q_MVVMCORE_EXPORT ServiceExistsException : public QTMVVM_EXCEPTION_BASE
{
public:
	//! @private
	ServiceExistsException(const QByteArray &iid);

	//! @inherit{QException::what}
	const char *what() const noexcept override;

	//! @inherit{QException::raise}
	virtual void raise() const QTMVVM_EXCEPTION_OR;
	//! @inherit{QException::clone}
	virtual QTMVVM_EXCEPTION_BASE *clone() const QTMVVM_EXCEPTION_OR;

protected:
	//! @private
	ServiceExistsException(const ServiceExistsException * const other);

	//! @private
	const QByteArray _msg;
};

//! Is thrown in case the construction of a service has failed
class Q_MVVMCORE_EXPORT ServiceConstructionException : public QTMVVM_EXCEPTION_BASE
{
public:
	//! @private
	ServiceConstructionException(const QByteArray &message);

	//! @inherit{QException::what}
	const char *what() const noexcept override;

	//! @inherit{QException::raise}
	virtual void raise() const QTMVVM_EXCEPTION_OR;
	//! @inherit{QException::clone}
	virtual QTMVVM_EXCEPTION_BASE *clone() const QTMVVM_EXCEPTION_OR;

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
	QTMVVM_EXCEPTION_BASE *clone() const override;

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
void ServiceRegistry::registerInterface(DestructionScope scope, bool weak)
{
	QTMVVM_SERVICE_ASSERT(TInterface, TService)
	registerService(qobject_interface_iid<TInterface*>(), &TService::staticMetaObject, scope, weak);
}

template <typename TInterface, typename TService, typename TFunc>
void ServiceRegistry::registerInterface(TFunc fn, DestructionScope scope, bool weak)
{
	QTMVVM_SERVICE_ASSERT(TInterface, TService)
	QByteArrayList injectables;
	auto packed_fn = __helpertypes::pack_function(std::move(fn), injectables);
	registerService(qobject_interface_iid<TInterface*>(), packed_fn, injectables, scope, weak);
}

template<typename TInterface, typename TService>
void ServiceRegistry::registerInterface(TService *service, DestructionScope scope, bool weak)
{
	QTMVVM_SERVICE_ASSERT(TInterface, TService)
	registerService(qobject_interface_iid<TInterface*>(), [service](const QObjectList &params) -> QObject* {
		Q_UNUSED(params);
		return service;
	}, QByteArrayList(), scope, weak);
}

#undef QTMVVM_SERVICE_ASSERT
#define QTMVVM_SERVICE_ASSERT(tsvc) \
	static_assert(__helpertypes::is_qobj<tsvc>::value, "TService must be a qobject class");

template<typename TService>
void ServiceRegistry::registerObject(DestructionScope scope, bool weak)
{
	QTMVVM_SERVICE_ASSERT(TService)
	registerService(__helpertypes::qobject_iid<TService*>(), &TService::staticMetaObject, scope, weak);
}

template<typename TService, typename TFunc>
void ServiceRegistry::registerObject(TFunc fn, DestructionScope scope, bool weak)
{
	QTMVVM_SERVICE_ASSERT(TService)
	QByteArrayList injectables;
	auto packed_fn = __helpertypes::pack_function(std::move(fn), injectables);
	registerService(__helpertypes::qobject_iid<TService*>(), packed_fn, injectables, scope, weak);
}

template<typename TService>
void ServiceRegistry::registerObject(TService *service, DestructionScope scope, bool weak)
{
	QTMVVM_SERVICE_ASSERT(TService)
	registerService(__helpertypes::qobject_iid<TService*>(), [service](const QObjectList &params) -> QObject* {
		Q_UNUSED(params);
		return service;
	}, QByteArrayList(), scope, weak);
}

#undef QTMVVM_SERVICE_ASSERT

template<typename TInterface>
void ServiceRegistry::registerPlugin(QString pluginType, QString pluginKey,  DestructionScope scope, bool weak)
{
	registerService(qobject_interface_iid<TInterface*>(), std::move(pluginType), std::move(pluginKey), scope, weak);
}

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

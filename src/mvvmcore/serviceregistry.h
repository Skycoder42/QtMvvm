#ifndef QTMVVM_SERVICEREGISTRY_H
#define QTMVVM_SERVICEREGISTRY_H

#include <functional>

#include <QtCore/qscopedpointer.h>
#include <QtCore/qvariant.h>
#include <QtCore/qexception.h>

#include "QtMvvmCore/qtmvvmcore_global.h"

namespace QtMvvm {

class ServiceRegistryPrivate;
class Q_MVVMCORE_EXPORT ServiceRegistry
{
public:
	ServiceRegistry(ServiceRegistryPrivate *d_ptr);

	static ServiceRegistry* instance();

	template <typename TInterface, typename TService>
	void registerInterface();
	template <typename TInterface, typename TService, typename TFunc>
	void registerInterface(const TFunc &fn);
	template <typename TInterface, typename TService>
	void registerInterface(TService *service);
	template <typename TService>
	void registerObject();
	template <typename TService, typename TFunc>
	void registerObject(const TFunc &fn);
	template <typename TService>
	void registerObject(TService *service);

	void registerService(const QByteArray &iid,
						 const QMetaObject *metaObject);
	void registerService(const QByteArray &iid,
						 const std::function<QObject*(const QObjectList &)> &fn,
						 QByteArrayList injectables);

	template <typename TInterface>
	TInterface *service();
	QObject *serviceObj(const QByteArray &iid);

private:
	friend class QtMvvm::ServiceRegistryPrivate;
	QScopedPointer<ServiceRegistryPrivate> d;
};

class Q_MVVMCORE_EXPORT ServiceExistsException : public QException
{
public:
	ServiceExistsException(const QByteArray &iid);

	const char *what() const noexcept override;

	void raise() const override;
	QException *clone() const override;

protected:
	ServiceExistsException(const ServiceExistsException * const other);

	const QByteArray _msg;
};

class Q_MVVMCORE_EXPORT ServiceConstructionException : public QException
{
public:
	ServiceConstructionException(const QByteArray &message);

	const char *what() const noexcept override;

	void raise() const override;
	QException *clone() const override;

protected:
	ServiceConstructionException(const ServiceConstructionException * const other);

	const QByteArray _msg;
};

class Q_MVVMCORE_EXPORT ServiceDependencyException : public ServiceConstructionException
{
public:
	ServiceDependencyException(const QByteArray &iid);

	void raise() const override;
	QException *clone() const override;

protected:
	ServiceDependencyException(const ServiceDependencyException * const other);
};

// ------------- Generic Implementation -------------

#define QTMVVM_SERVICE_ASSERT(tint, tsvc) \
	static_assert(__helpertypes::is_valid_interface<TInterface, TService>::value, "TService must implement the given TInterface interface and be a qobject class"); \
	Q_ASSERT_X(qobject_interface_iid<TInterface*>(), Q_FUNC_INFO, "TInterface must be registered with Q_DECLARE_INTERFACE");

template<typename TInterface, typename TService>
void ServiceRegistry::registerInterface()
{
	QTMVVM_SERVICE_ASSERT(TInterface, TService)
	registerService(qobject_interface_iid<TInterface*>(), &TService::staticMetaObject);
}

template <typename TInterface, typename TService, typename TFunc>
void ServiceRegistry::registerInterface(const TFunc &fn)
{
	QTMVVM_SERVICE_ASSERT(TInterface, TService)
	QByteArrayList injectables;
	auto packed_fn = __helpertypes::pack_function(fn, injectables);
	registerService(qobject_interface_iid<TInterface*>(), packed_fn, injectables);
}

template<typename TInterface, typename TService>
void ServiceRegistry::registerInterface(TService *service)
{
	QTMVVM_SERVICE_ASSERT(TInterface, TService)
	registerService(qobject_interface_iid<TInterface*>(), [service](const QObjectList &params) -> QObject* {
		Q_UNUSED(params);
		return service;
	}, QByteArrayList());
}

#undef QTMVVM_SERVICE_ASSERT
#define QTMVVM_SERVICE_ASSERT(tsvc) \
	static_assert(__helpertypes::is_qobj<tsvc>::value, "TService must be a qobject class");

template<typename TService>
void ServiceRegistry::registerObject()
{
	QTMVVM_SERVICE_ASSERT(TService)
	registerService(__helpertypes::qobject_iid<TService*>(), &TService::staticMetaObject);
}

template<typename TService, typename TFunc>
void ServiceRegistry::registerObject(const TFunc &fn)
{
	QTMVVM_SERVICE_ASSERT(TService)
	QByteArrayList injectables;
	auto packed_fn = __helpertypes::pack_function(fn, injectables);
	registerService(__helpertypes::qobject_iid<TService*>(), packed_fn, injectables);
}

template<typename TService>
void ServiceRegistry::registerObject(TService *service)
{
	QTMVVM_SERVICE_ASSERT(TService)
	registerService(__helpertypes::qobject_iid<TService*>(), [service](const QObjectList &params) -> QObject* {
		Q_UNUSED(params);
		return service;
	}, QByteArrayList());
}

#undef QTMVVM_SERVICE_ASSERT

template<typename TInterface>
TInterface *ServiceRegistry::service()
{
	return qobject_cast<TInterface*>(serviceObj(__helpertypes::inject_iid<TInterface*>()));
}

}

#endif // QTMVVM_SERVICEREGISTRY_H

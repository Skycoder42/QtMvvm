#ifndef QTMVVM_SERVICEREGISTRY_H
#define QTMVVM_SERVICEREGISTRY_H

#include <functional>

#include <QtCore/qscopedpointer.h>
#include <QtCore/qvariant.h>

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
						 const std::function<QObject*(const QVariantList&)> &fn,
						 QByteArrayList injectables);

private:
	QScopedPointer<ServiceRegistryPrivate> d;
};

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
	registerService(qobject_interface_iid<TInterface*>(), [service](const QVariantList &params) -> QObject* {
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
	registerService(__helpertypes::qobject_iid<TService*>(), [service](const QVariantList &params) -> QObject* {
		Q_UNUSED(params);
		return service;
	}, QByteArrayList());
}

#undef QTMVVM_SERVICE_ASSERT

}

#endif // QTMVVM_SERVICEREGISTRY_H

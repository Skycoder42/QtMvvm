#ifndef QTMVVM_SERVICEREGISTRY_H
#define QTMVVM_SERVICEREGISTRY_H

#include <functional>

#include <QtCore/qscopedpointer.h>

#include "QtMvvmCore/qtmvvmcore_global.h"

namespace QtMvvm {

class ServiceRegistryPrivate;
class Q_MVVMCORE_EXPORT ServiceRegistry
{
public:
	ServiceRegistry(ServiceRegistryPrivate *d_ptr);

	static ServiceRegistry* instance();

	template <typename TInterface, typename TService>
	void registerService();
	template <typename TInterface, typename TService, typename... Injectables>
	void registerService(const std::function<TService*(Injectables...)> &fn);
	template <typename TInterface, typename TService>
	void registerService(TService *service);
	template <typename TService>
	void registerService();
	template <typename TService, typename... Injectables>
	void registerService(const std::function<TService*(Injectables...)> &fn);
	template <typename TService>
	void registerService(TService *service);

	void registerService(const QByteArray &iid,
						 const QMetaObject *metaObject);
	void registerService(const QByteArray &iid,
						 const std::function<QObject*(const QVariantList&)> &fn,
						 QByteArrayList injectables);

private:
	QScopedPointer<ServiceRegistryPrivate> d;
};

}

#endif // QTMVVM_SERVICEREGISTRY_H

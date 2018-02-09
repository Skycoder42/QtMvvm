#include "serviceregistry.h"
#include "serviceregistry_p.h"

#include <QtCore/QGlobalStatic>

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
	//TODO create function with injectables and pass to other overload
}

void ServiceRegistry::registerService(const QByteArray &iid, const std::function<QObject *(const QVariantList &)> &fn, QByteArrayList injectables)
{

}

// ------------- Private Implementation -------------

ServiceRegistryPrivate::ServiceRegistryPrivate()
{

}

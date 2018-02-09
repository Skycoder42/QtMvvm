#include "serviceregistry.h"
#include "serviceregistry_p.h"

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
	Q_ASSERT(!d->services.contains(iid)); //TODO exception
	d->services.insert(iid, QSharedPointer<ServiceRegistryPrivate::MetaServiceInfo>::create(metaObject));
}

void ServiceRegistry::registerService(const QByteArray &iid, const std::function<QObject*(const QObjectList &)> &fn, QByteArrayList injectables)
{
	QMutexLocker _(&d->serviceMutex);
	Q_ASSERT(!d->services.contains(iid)); //TODO exception
	d->services.insert(iid, QSharedPointer<ServiceRegistryPrivate::FnServiceInfo>::create(fn, injectables));
}

// ------------- Private Implementation -------------

ServiceRegistryPrivate::ServiceRegistryPrivate() :
	serviceMutex(),
	services()
{}



ServiceRegistryPrivate::ServiceInfo::ServiceInfo() :
	_instance(nullptr)
{}

ServiceRegistryPrivate::ServiceInfo::~ServiceInfo()
{
	if(_instance)
		QMetaObject::invokeMethod(_instance, "deleteLater");
}

QObject *ServiceRegistryPrivate::ServiceInfo::instance(ServiceRegistryPrivate *d)
{
	if(!_instance)
		_instance = construct(d);
	return _instance;
}



ServiceRegistryPrivate::FnServiceInfo::FnServiceInfo(const std::function<QObject*(QObjectList)> &creator, const QByteArrayList &injectables) :
	creator(creator),
	injectables(injectables)
{}

QObject *ServiceRegistryPrivate::FnServiceInfo::construct(ServiceRegistryPrivate *d) const
{
	QObjectList params;
	foreach(auto inj, injectables)
		params.append(d->services.value(inj)->instance(d));//TODO exception
	return creator(params);
}



ServiceRegistryPrivate::MetaServiceInfo::MetaServiceInfo(const QMetaObject *metaObject) :
	metaObject(metaObject)
{}

QObject *ServiceRegistryPrivate::MetaServiceInfo::construct(ServiceRegistryPrivate *d) const
{
	auto instance = metaObject->newInstance();
	Q_ASSERT(instance); //TODO exception

	static QRegularExpression nameRegex(QStringLiteral(R"__(^__qtmvvm_inject_(.+)$)__"),
										QRegularExpression::OptimizeOnFirstUsageOption);

	for(auto i = 0; i < metaObject->propertyCount(); i++) {
		auto prop = metaObject->property(i);
		auto match = nameRegex.match(QString::fromUtf8(prop.name()));
		if(match.hasMatch()) {
			auto tPropIndex = metaObject->indexOfProperty(qUtf8Printable(match.captured(1)));
			if(tPropIndex == -1)
				continue; //TODO warning
			auto tProp = metaObject->property(tPropIndex);

			auto inject = prop.read(instance).toByteArray();
			auto injObj = d->services.value(inject)->instance(d); //TODO exception
			tProp.write(instance, QVariant::fromValue(injObj)); //TODO check if casting works
		}
	}

	//TODO call "init" method
	return instance;
}

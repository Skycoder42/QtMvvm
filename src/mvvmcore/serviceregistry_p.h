#ifndef QTMVVM_SERVICEREGISTRY_P_H
#define QTMVVM_SERVICEREGISTRY_P_H

#include <QtCore/QMutex>

#include "qtmvvmcore_global.h"
#include "serviceregistry.h"

namespace QtMvvm {

class ServiceRegistryPrivate
{
public:
	class ServiceInfo {
	public:
		ServiceInfo();
		virtual ~ServiceInfo();

		QObject *instance(ServiceRegistryPrivate *d);

	protected:
		virtual QObject *construct(ServiceRegistryPrivate *d) const = 0;

	private:
		QObject *_instance;
	};

	class FnServiceInfo : public ServiceInfo {
	public:
		FnServiceInfo(const std::function<QObject*(QObjectList)> &creator, const QByteArrayList &injectables);

	protected:
		QObject *construct(ServiceRegistryPrivate *d) const final;

	private:
		std::function<QObject*(QObjectList)> creator;
		QByteArrayList injectables;
	};

	class MetaServiceInfo : public ServiceInfo {
	public:
		MetaServiceInfo(const QMetaObject *metaObject);

	protected:
		QObject *construct(ServiceRegistryPrivate *d) const final;

	private:
		const QMetaObject *metaObject;
	};

	QMutex serviceMutex;
	QHash<QByteArray, QSharedPointer<ServiceInfo>> services;

	ServiceRegistryPrivate();
};

}

#endif // QTMVVM_SERVICEREGISTRY_P_H

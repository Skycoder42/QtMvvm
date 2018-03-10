#ifndef QTMVVM_SERVICEREGISTRY_P_H
#define QTMVVM_SERVICEREGISTRY_P_H

#include <QtCore/QMutex>
#include <QtCore/QSharedPointer>

#include "qtmvvmcore_global.h"
#include "serviceregistry.h"

namespace QtMvvm {

class ServiceRegistryPrivate
{
public:
	class ServiceInfo {
	public:
		ServiceInfo(bool weak);
		virtual ~ServiceInfo();

		bool replaceable() const;
		QObject *instance(ServiceRegistryPrivate *d, const QByteArray &iid);

	protected:
		virtual QObject *construct(ServiceRegistryPrivate *d) const = 0;

	private:
		const bool _weak;
		QObject *_instance;
	};

	class FnServiceInfo : public ServiceInfo {
	public:
		FnServiceInfo(const std::function<QObject*(QObjectList)> &creator, const QByteArrayList &injectables, bool weak);

	protected:
		QObject *construct(ServiceRegistryPrivate *d) const final;

	private:
		std::function<QObject*(QObjectList)> creator;
		QByteArrayList injectables;
	};

	class MetaServiceInfo : public ServiceInfo {
	public:
		MetaServiceInfo(const QMetaObject *metaObject, bool weak);

	protected:
		QObject *construct(ServiceRegistryPrivate *d) const final;

	private:
		const QMetaObject *metaObject;
	};

	QMutex serviceMutex;
	QHash<QByteArray, QSharedPointer<ServiceInfo>> services;

	bool serviceBlocked(const QByteArray &iid) const;
	QObject *constructInjectedLocked(const QMetaObject *metaObject, QObject *parent);
	void injectLocked(QObject *object);
};

}

#endif // QTMVVM_SERVICEREGISTRY_P_H

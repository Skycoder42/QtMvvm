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
		Q_DISABLE_COPY(ServiceInfo)
	public:
		ServiceInfo(bool weak, ServiceRegistry::DestructionScope scope);
		virtual ~ServiceInfo();

		bool replaceable() const;
		bool needsDestroy(ServiceRegistry::DestructionScope scope) const;
		QObject *instance(ServiceRegistryPrivate *d, const QByteArray &iid);

	protected:
		virtual QObject *construct(ServiceRegistryPrivate *d) const = 0;

	private:
		const bool _weak;
		const ServiceRegistry::DestructionScope _scope;
		QObject *_instance = nullptr;
		mutable bool _closingDown = false;
	};

	class FnServiceInfo : public ServiceInfo {
	public:
		FnServiceInfo(std::function<QObject*(QObjectList)> creator, QByteArrayList injectables, bool weak, ServiceRegistry::DestructionScope scope);

	protected:
		QObject *construct(ServiceRegistryPrivate *d) const final;

	private:
		std::function<QObject*(QObjectList)> creator;
		QByteArrayList injectables;
	};

	class MetaServiceInfo : public ServiceInfo {
	public:
		MetaServiceInfo(const QMetaObject *metaObject, bool weak, ServiceRegistry::DestructionScope scope);

	protected:
		QObject *construct(ServiceRegistryPrivate *d) const final;

	private:
		const QMetaObject *metaObject;
	};

	QMutex serviceMutex{QMutex::Recursive};
	QHash<QByteArray, QSharedPointer<ServiceInfo>> services;

	bool serviceBlocked(const QByteArray &iid) const;
	QObject *constructInjectedLocked(const QMetaObject *metaObject, QObject *parent);
	void injectLocked(QObject *object);

	void destroyServices(ServiceRegistry::DestructionScope scope);

	static void appDestroyedHook();
};

}

#endif // QTMVVM_SERVICEREGISTRY_P_H

#ifndef QTMVVM_QQMLSERVICEREGISTRY_H
#define QTMVVM_QQMLSERVICEREGISTRY_H

#include <QtCore/QObject>

#include <QtMvvmCore/ServiceRegistry>

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlComponent>

namespace QtMvvm {

class QQmlServiceRegistry : public QObject
{
	Q_OBJECT

public:
	enum DestructionScope {
		DestroyOnAppQuit = ServiceRegistry::DestroyOnAppQuit,
		DestroyOnAppDestroy = ServiceRegistry::DestroyOnAppDestroy,
		DestroyOnRegistryDestroy = ServiceRegistry::DestroyOnRegistryDestroy,

		DestroyNever = ServiceRegistry::DestroyNever
	};
	Q_ENUM(DestructionScope)

	explicit QQmlServiceRegistry(QQmlEngine *parent = nullptr);

	Q_INVOKABLE bool isRegistered(const QString &iid) const;

	Q_INVOKABLE void registerObject(QObject *object, QtMvvm::QQmlServiceRegistry::DestructionScope scope = DestroyOnAppQuit, bool weak = false);
	Q_INVOKABLE void registerObject(const QString &iid, const QJSValue &function, bool weak = false);
	Q_INVOKABLE void registerObject(const QUrl &componentUrl, bool weak = false);

	Q_INVOKABLE void registerPlugin(const QString &iid,
									QString pluginType = {},
									QString pluginKey = {},
									DestructionScope scope = DestroyOnAppDestroy,
									bool weak = false);

	Q_INVOKABLE QObject *service(const QString &iid);

private:
	QQmlEngine *_engine;
};

}

#endif // QTMVVM_QQMLSERVICEREGISTRY_H

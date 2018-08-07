#include "qqmlserviceregistry.h"
#include <QtQml/QQmlInfo>
using namespace QtMvvm;

QQmlServiceRegistry::QQmlServiceRegistry(QQmlEngine *parent) :
	QObject{parent},
	_engine{parent}
{}

bool QQmlServiceRegistry::isRegistered(const QString &iid) const
{
	return ServiceRegistry::instance()->isRegistered(iid.toUtf8());
}

void QQmlServiceRegistry::registerObject(QObject *object, DestructionScope scope, bool weak)
{
	ServiceRegistry::instance()->registerService(__helpertypes::InjectablePrefix + object->metaObject()->className(),
												 [object](const QObjectList &) {
		return object;
	}, {}, static_cast<ServiceRegistry::DestructionScope>(scope), weak);
}

void QQmlServiceRegistry::registerObject(const QString &iid, const QJSValue &function, bool weak)
{
	if(!function.isCallable()) {
		qmlWarning(this) << "function must be callable";
		return;
	}
	ServiceRegistry::instance()->registerService(iid.toUtf8(), [function](const QObjectList &) {
		auto fn = function;
		return fn.call().toQObject();
	}, {}, ServiceRegistry::DestroyOnAppQuit, weak);
}

void QQmlServiceRegistry::registerObject(const QUrl &componentUrl, bool weak)
{
	ServiceRegistry::instance()->registerService(componentUrl.toString().toUtf8(), [this, componentUrl](const QObjectList &) -> QObject* {
		QQmlComponent component{_engine, componentUrl, QQmlComponent::PreferSynchronous};
		switch(component.status()) {
		case QQmlComponent::Ready:
		{
			auto object = component.create();
			QQmlEngine::setObjectOwnership(object, QQmlEngine::CppOwnership);
			object->setParent(nullptr);
			return object;
		}
		case QQmlComponent::Null:
			throw ServiceConstructionException{"No component was loaded for URL: " +
											   component.url().toString().toUtf8()};
		case QQmlComponent::Error:
			throw ServiceConstructionException{"Failed to load componentfor URL \"" + component.url().toString().toUtf8() +
											   "\" with error: " + component.errorString().trimmed().toUtf8()};
		case QQmlComponent::Loading:
			throw ServiceConstructionException{"Unable to construct service from asynchronously loaded URL: " +
											   component.url().toString().toUtf8()};
		default:
			Q_UNREACHABLE();
			return nullptr;
		}
	}, {}, ServiceRegistry::DestroyOnAppQuit, weak);
}

void QQmlServiceRegistry::registerPlugin(const QString &iid, QString pluginType, QString pluginKey, QQmlServiceRegistry::DestructionScope scope, bool weak)
{
	ServiceRegistry::instance()->registerService(iid.toUtf8(),
												 std::move(pluginType),
												 std::move(pluginKey),
												 static_cast<ServiceRegistry::DestructionScope>(scope),
												 weak);
}

QObject *QQmlServiceRegistry::service(const QString &iid)
{
	return ServiceRegistry::instance()->serviceObj(iid.toUtf8());
}

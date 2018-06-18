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
	}, {}, weak, ServiceRegistry::DestroyOnAppQuit);
}

QObject *QQmlServiceRegistry::service(const QString &iid)
{
	return ServiceRegistry::instance()->serviceObj(iid.toUtf8());
}

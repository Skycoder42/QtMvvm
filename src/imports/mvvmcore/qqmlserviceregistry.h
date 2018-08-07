#ifndef QTMVVM_QQMLSERVICEREGISTRY_H
#define QTMVVM_QQMLSERVICEREGISTRY_H

#include <QtCore/QObject>

#include <QtMvvmCore/ServiceRegistry>

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlComponent>

#ifdef DOXYGEN_RUN
namespace de::skycoder42::QtMvvm::Core {

/*! @brief A QML singleton to access some QtMvvm::ServiceRegistry functionality
 *
 * @extends QtQml.QtObject
 * @since 1.1
 *
 * @sa QtMvvm::ServiceRegistry
 */
class ServiceRegistry
#else
namespace QtMvvm {

class QQmlServiceRegistry : public QObject
#endif
{
	Q_OBJECT

public:
	//! @copybrief QtMvvm::ServiceRegistry
	enum DestructionScope {
		DestroyOnAppQuit = ServiceRegistry::DestroyOnAppQuit, //!< @copybrief QtMvvm::ServiceRegistry::DestroyOnAppQuit
		DestroyOnAppDestroy = ServiceRegistry::DestroyOnAppDestroy, //!< @copybrief QtMvvm::ServiceRegistry::DestroyOnAppDestroy
		DestroyOnRegistryDestroy = ServiceRegistry::DestroyOnRegistryDestroy, //!< @copybrief QtMvvm::ServiceRegistry::DestroyOnRegistryDestroy
		DestroyNever = ServiceRegistry::DestroyNever //!< @copybrief QtMvvm::ServiceRegistry::DestroyNever
	};
	Q_ENUM(DestructionScope)

	//! @private
	explicit QQmlServiceRegistry(QQmlEngine *parent = nullptr);

	//! @copydoc QtMvvm::ServiceRegistry::isRegistered() const
	Q_INVOKABLE bool isRegistered(const QString &iid) const;

	//! @copydoc QtMvvm::ServiceRegistry::registerObject(TService *, DestructionScope, bool)
	Q_INVOKABLE void registerObject(QObject *service, QtMvvm::QQmlServiceRegistry::DestructionScope scope = DestroyOnAppQuit, bool weak = false);
	/*!
	 * @brief Register a service via a constructor function
	 *
	 * @param iid The interface id of the type to register the service for
	 * @param function The function to be called to construct the service
	 * @param weak Specifies if the registration should be a weak one or a normal one
	 *
	 * If the function returns successfully, from now on a service of the given type can be accessed
	 * via the registry for the interface. The service is lazy initialized an will be created as soon
	 * as it is requested for the first time. It is created by calling the given `function`.
	 *
	 * If the service is registered as weak, registering another service for the same iid will not
	 * throw an exception but instead discard (and delete) this one.
	 */
	Q_INVOKABLE void registerObject(const QString &iid, const QJSValue &function, bool weak = false);
	/*!
	 * @brief Register a qml component as service object
	 *
	 * @param componentUrl The URL of the component to be created
	 * @param weak Specifies if the registration should be a weak one or a normal one
	 *
	 * This method works similar to the other register methods, but is special in that it
	 * allows you to register a qml component. On construction, that component is loaded
	 * synchronously and then insanciated to create an instance of the contained object.
	 *
	 * Unlike the other methods, the destruction scope for those is always
	 * ServiceRegistry::DestroyOnAppDestroy, as the object will depend on the QML engine
	 *
	 * If the service is registered as weak, registering another service for the same iid will not
	 * throw an exception but instead discard (and delete) this one.
	 */
	Q_INVOKABLE void registerObject(const QUrl &componentUrl, bool weak = false);

	//! @copydoc QtMvvm::ServiceRegistry::registerService(QByteArray, QString, QString, DestructionScope, bool)
	Q_INVOKABLE void registerPlugin(const QString &iid,
									QString pluginType = {},
									QString pluginKey = {},
									DestructionScope scope = DestroyOnAppDestroy,
									bool weak = false);

	//! @copydoc QtMvvm::ServiceRegistry::serviceObj(const QByteArray &)
	Q_INVOKABLE QObject *service(const QString &iid);

private:
	QQmlEngine *_engine;
};

}

#endif // QTMVVM_QQMLSERVICEREGISTRY_H

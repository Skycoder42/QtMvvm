#ifndef QTMVVM_INPUTVIEWFACTORY_H
#define QTMVVM_INPUTVIEWFACTORY_H

#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qobject.h>

#include "QtMvvmQuick/qtmvvmquick_global.h"

namespace QtMvvm {

class InputViewFactoryPrivate;
class Q_MVVMQUICK_EXPORT InputViewFactory : public QObject
{
	Q_OBJECT

public:
	InputViewFactory();
	virtual ~InputViewFactory();

	Q_INVOKABLE virtual QUrl getInputUrl(const QByteArray &type, const QVariantMap &viewProperties);
	Q_INVOKABLE virtual QUrl getDelegate(const QByteArray &type, const QVariantMap &viewProperties);

	Q_INVOKABLE virtual void addSimpleInput(const QByteArray &type, const QUrl &qmlFileUrl);
	template <typename TType>
	inline void addSimpleInput(const QUrl &qmlFileUrl);

	Q_INVOKABLE virtual void addSimpleDelegate(const QByteArray &type, const QUrl &qmlFileUrl);
	template <typename TType>
	inline void addSimpleDelegate(const QUrl &qmlFileUrl);

	Q_INVOKABLE virtual void addInputAlias(const QByteArray &alias, const QByteArray &targetType);
	template <typename TAliasType, typename TTargetType>
	inline void addInputAlias();

	Q_INVOKABLE virtual void addDelegateAlias(const QByteArray &alias, const QByteArray &targetType);
	template <typename TAliasType, typename TTargetType>
	inline void addDelegateAlias();

private:
	QScopedPointer<InputViewFactoryPrivate> d;
};

template<typename TType>
inline void InputViewFactory::addSimpleInput(const QUrl &qmlFileUrl)
{
	addSimpleInput(QMetaType::typeName(qMetaTypeId<TType>()), qmlFileUrl);
}

template<typename TType>
void InputViewFactory::addSimpleDelegate(const QUrl &qmlFileUrl)
{
	addSimpleDelegate(QMetaType::typeName(qMetaTypeId<TType>()), qmlFileUrl);
}

template<typename TAliasType, typename TTargetType>
inline void InputViewFactory::addInputAlias()
{
	addInputAlias(QMetaType::typeName(qMetaTypeId<TAliasType>()), QMetaType::typeName(qMetaTypeId<TTargetType>()));
}

template<typename TAliasType, typename TTargetType>
void InputViewFactory::addDelegateAlias()
{
	addDelegateAlias(QMetaType::typeName(qMetaTypeId<TAliasType>()), QMetaType::typeName(qMetaTypeId<TTargetType>()));
}

}

#endif // QTMVVM_INPUTVIEWFACTORY_H

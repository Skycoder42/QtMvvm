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

	Q_INVOKABLE virtual void addSimpleView(const QByteArray &type, const QUrl &qmlFileUrl);
	template <typename TType>
	inline void addSimpleView(const QUrl &qmlFileUrl);

	Q_INVOKABLE virtual void addAlias(const QByteArray &alias, const QByteArray &targetType);
	template <typename TAliasType, typename TTargetType>
	inline void addAlias();

private:
	QScopedPointer<InputViewFactoryPrivate> d;
};

template<typename TType>
inline void InputViewFactory::addSimpleView(const QUrl &qmlFileUrl)
{
	addSimpleView(QMetaType::typeName(qMetaTypeId<TType>()), qmlFileUrl);
}

template<typename TAliasType, typename TTargetType>
inline void InputViewFactory::addAlias()
{
	addAlias(QMetaType::typeName(qMetaTypeId<TAliasType>()), QMetaType::typeName(qMetaTypeId<TTargetType>()));
}

}

#endif // QTMVVM_INPUTVIEWFACTORY_H

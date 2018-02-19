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

	//virtual int metaTypeId(const QByteArray &type, const QVariantMap &properties);
	Q_INVOKABLE virtual QUrl getInputUrl(const QByteArray &type, const QVariantMap &viewProperties);

	Q_INVOKABLE virtual bool addSimpleView(const QByteArray &type, const QUrl &qmlFileUrl);
	template <typename T>
	bool addSimpleView(const QUrl &qmlFileUrl);

private:
	QScopedPointer<InputViewFactoryPrivate> d;
};

template<typename T>
bool InputViewFactory::addSimpleView(const QUrl &qmlFileUrl)
{
	return addSimpleView(QMetaType::typeName(qMetaTypeId<T>()), qmlFileUrl);
}

}

#endif // QTMVVM_INPUTVIEWFACTORY_H

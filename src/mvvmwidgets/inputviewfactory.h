#ifndef QTMVVM_INPUTVIEWFACTORY_H
#define QTMVVM_INPUTVIEWFACTORY_H

#include <functional>

#include <QtCore/qscopedpointer.h>

#include <QtWidgets/qwidget.h>

#include "QtMvvmWidgets/qtmvvmwidgets_global.h"

namespace QtMvvm {

class InputViewFactoryPrivate;
class InputViewFactory
{
public:
	InputViewFactory();
	virtual ~InputViewFactory();

	virtual QWidget *createInput(const QByteArray &type, QWidget *parent, const QVariantMap &viewProperties);

	virtual void addSimpleWidget(const QByteArray &type, const std::function<QWidget*(QWidget*)> &creator);
	template <typename TType, typename TWidget>
	void addSimpleWidget();

private:
	QScopedPointer<InputViewFactoryPrivate> d;
};

template<typename TType, typename TWidget>
void InputViewFactory::addSimpleWidget()
{
	addSimpleWidget(QMetaType::typeName(qMetaTypeId<TType>()), [](QWidget *parent){
		return new TWidget(parent);
	});
}

}

#endif // QTMVVM_INPUTVIEWFACTORY_H

#ifndef QTMVVM_INPUTWIDGETFACTORY_H
#define QTMVVM_INPUTWIDGETFACTORY_H

#include <functional>

#include <QtCore/qscopedpointer.h>

#include <QtWidgets/qwidget.h>

#include "QtMvvmWidgets/qtmvvmwidgets_global.h"

namespace QtMvvm {

class InputWidgetFactoryPrivate;
class InputWidgetFactory
{
public:
	InputWidgetFactory();
	virtual ~InputWidgetFactory();

	virtual QWidget *createInput(const QByteArray &type, QWidget *parent, const QVariantMap &viewProperties);

	virtual void addSimpleWidget(const QByteArray &type, const std::function<QWidget*(QWidget*)> &creator);
	template <typename TType, typename TWidget>
	inline void addSimpleWidget();

	virtual void addAlias(const QByteArray &alias, const QByteArray &targetType);
	template <typename TAliasType, typename TTargetType>
	inline void addAlias();

private:
	QScopedPointer<InputWidgetFactoryPrivate> d;
};

template<typename TType, typename TWidget>
inline void InputWidgetFactory::addSimpleWidget()
{
	addSimpleWidget(QMetaType::typeName(qMetaTypeId<TType>()), [](QWidget *parent){
		return new TWidget(parent);
	});
}

template<typename TAliasType, typename TTargetType>
inline void InputWidgetFactory::addAlias()
{
	addAlias(QMetaType::typeName(qMetaTypeId<TAliasType>()), QMetaType::typeName(qMetaTypeId<TTargetType>()));
}

}

#endif // QTMVVM_INPUTWIDGETFACTORY_H

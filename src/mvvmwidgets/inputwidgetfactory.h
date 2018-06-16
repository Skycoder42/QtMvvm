#ifndef QTMVVM_INPUTWIDGETFACTORY_H
#define QTMVVM_INPUTWIDGETFACTORY_H

#include <functional>

#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>

#include <QtWidgets/qwidget.h>

#include "QtMvvmWidgets/qtmvvmwidgets_global.h"

namespace QtMvvm {

class InputWidgetFactoryPrivate;
//! A factory class to generate input edit widgets by their type names
class InputWidgetFactory : public QObject
{
	Q_OBJECT

public:
	//! Default constructor
	Q_INVOKABLE explicit InputWidgetFactory(QObject *parent = nullptr);
	~InputWidgetFactory() override;

	//! Create a new input widget of the given input type
	virtual QWidget *createInput(const QByteArray &type, QWidget *parent, const QVariantMap &viewProperties);

	//! Adds a new generator to create widgets for the given type
	template <typename TType, typename TWidget>
	inline void addSimpleWidget();
	//! @copybrief addSimpleWidget()
	virtual void addSimpleWidget(const QByteArray &type, const std::function<QWidget*(QWidget*)> &creator);

	//! Adds a type name alias
	template <typename TAliasType, typename TTargetType>
	inline void addAlias();
	//! @copybrief addAlias()
	virtual void addAlias(const QByteArray &alias, const QByteArray &targetType);

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

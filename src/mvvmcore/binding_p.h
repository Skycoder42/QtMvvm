#ifndef QTMVVM_BINDING_P_H
#define QTMVVM_BINDING_P_H

#include <QtCore/QObject>
#include <QtCore/QPointer>

#include "qtmvvmcore_global.h"
#include "binding.h"

namespace QtMvvm {

class BindingPrivate : public QObject
{
	friend class QtMvvm::Binding;
	Q_OBJECT

public:
	static Binding bind(QObject *viewModel, QMetaProperty viewModelProperty,
						QObject *view, QMetaProperty viewProperty,
						Binding::BindingDirection type,
						QMetaMethod viewModelChangeSignal,
						QMetaMethod viewChangeSignal);

private Q_SLOTS:
	void viewModelTrigger();
	void viewTrigger();

private:
	BindingPrivate(QObject *viewModel, QMetaProperty viewModelProperty,
				   QObject *view, QMetaProperty viewProperty);

	void init();
	void bindFrom(QMetaMethod changeSignal);
	void bindTo(QMetaMethod changeSignal);

	QPointer<QObject> viewModel;
	QPointer<QObject> view;
	QMetaProperty viewModelProperty;
	QMetaProperty viewProperty;

	QMetaObject::Connection modelToViewConnection;
	QMetaObject::Connection viewToModelConnection;

	void testReadable(const QMetaProperty &property, bool asView) const;
	void testWritable(const QMetaProperty &property, bool asView) const;
	void testNotifier(const QMetaProperty &property, bool asView) const;
};

}

#endif // QTMVVM_BINDING_P_H

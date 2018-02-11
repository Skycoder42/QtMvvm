#ifndef QTMVVM_BINDING_H
#define QTMVVM_BINDING_H

#include <QtCore/qobject.h>
#include <QtCore/qpointer.h>
#include <QtCore/qmetaobject.h>

#include "QtMvvmCore/qtmvvmcore_global.h"

namespace QtMvvm {

class BindingPrivate;
class Q_MVVMCORE_EXPORT Binding
{
	Q_GADGET

	Q_PROPERTY(bool valid READ isValid CONSTANT FINAL)

public:
	enum BindingDirectionFlag {
		SingleInit = 0x01,
		OneWayToView = (0x02 | SingleInit),
		OneWayToViewModel = 0x04,
		TwoWay = (OneWayToView | OneWayToViewModel)
	};
	Q_DECLARE_FLAGS(BindingDirection, BindingDirectionFlag)
	Q_FLAG(BindingDirection)

	Binding(QPointer<BindingPrivate> d_ptr = nullptr);
	~Binding();

	bool isValid() const;

private:
	QPointer<BindingPrivate> d;
};

Q_MVVMCORE_EXPORT Binding bind(QObject *viewModel, const char *viewModelProperty,
							   QObject *view, const char *viewProperty,
							   Binding::BindingDirection type = Binding::TwoWay,
							   const char *viewModelChangeSignal = nullptr,
							   const char *viewChangeSignal = nullptr);
Q_MVVMCORE_EXPORT Binding bind(QObject *viewModel, const QMetaProperty &viewModelProperty,
							   QObject *view, const QMetaProperty &viewProperty,
							   Binding::BindingDirection type = Binding::TwoWay,
							   const char *viewModelChangeSignal = nullptr,
							   const char *viewChangeSignal = nullptr);
Q_MVVMCORE_EXPORT Binding bind(QObject *viewModel, const QMetaProperty &viewModelProperty,
							   QObject *view, const QMetaProperty &viewProperty,
							   Binding::BindingDirection type = Binding::TwoWay,
							   const QMetaMethod &viewModelChangeSignal = {},
							   const QMetaMethod &viewChangeSignal = {});

}

Q_DECLARE_OPERATORS_FOR_FLAGS(QtMvvm::Binding::BindingDirection)

#endif // QTMVVM_BINDING_H

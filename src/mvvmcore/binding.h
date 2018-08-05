#ifndef QTMVVM_BINDING_H
#define QTMVVM_BINDING_H

#include <QtCore/qobject.h>
#include <QtCore/qpointer.h>
#include <QtCore/qmetaobject.h>

#include "QtMvvmCore/qtmvvmcore_global.h"

namespace QtMvvm {

class BindingPrivate;
//! A handle to a multidirectional binding between properties
class Q_MVVMCORE_EXPORT Binding
{
	Q_GADGET

public:
	//! Flags to control how to bind the properties
	enum BindingDirectionFlag {
		SingleInit = 0x01, //!< Only initialize the view by settings it to the viewmodels value on creation
		OneWayToView = (0x02 | SingleInit), //!< All changes done to the viewmodel are propagated to the view
		OneWayToViewModel = 0x04, //!< All changes done to the view are propagated to the viewmodel
		TwoWay = (OneWayToView | OneWayToViewModel) //!< All changes on both sides are propagated to the other one.
	};
	Q_DECLARE_FLAGS(BindingDirection, BindingDirectionFlag)
	Q_FLAG(BindingDirection)

	Binding();
	//! @private
	Binding(QPointer<BindingPrivate> d_ptr);
	~Binding();

	//! Specifies whether the binding is a valid and active binding
	bool isValid() const;
	//! @copydoc Binding::isValid
	operator bool() const;

	//! Remove the binding by disconnecting all change signals
	Q_INVOKABLE void unbind();

private:
	QPointer<BindingPrivate> d;
};

//! Create a multidirectional binding between properties
Q_MVVMCORE_EXPORT Binding bind(QObject *viewModel, const char *viewModelProperty,
							   QObject *view, const char *viewProperty,
							   Binding::BindingDirection type = Binding::TwoWay,
							   const char *viewModelChangeSignal = nullptr,
							   const char *viewChangeSignal = nullptr);
//! @copydoc QtMvvm::bind(QObject*, const char*, QObject*, const char*, Binding::BindingDirection, const char*, const char*)
Q_MVVMCORE_EXPORT Binding bind(QObject *viewModel, const QMetaProperty &viewModelProperty,
							   QObject *view, const QMetaProperty &viewProperty,
							   Binding::BindingDirection type = Binding::TwoWay,
							   const char *viewModelChangeSignal = nullptr,
							   const char *viewChangeSignal = nullptr);
//! @copydoc QtMvvm::bind(QObject*, const char*, QObject*, const char*, Binding::BindingDirection, const char*, const char*)
Q_MVVMCORE_EXPORT Binding bind(QObject *viewModel, const QMetaProperty &viewModelProperty,
							   QObject *view, const QMetaProperty &viewProperty,
							   Binding::BindingDirection type = Binding::TwoWay,
							   const QMetaMethod &viewModelChangeSignal = {},
							   const QMetaMethod &viewChangeSignal = {});

}

Q_DECLARE_OPERATORS_FOR_FLAGS(QtMvvm::Binding::BindingDirection)

#if defined(QT_BUILD_MVVMCORE_LIB) && defined(Q_MOC_RUN)
#include "binding_p.h"
#endif

#endif // QTMVVM_BINDING_H

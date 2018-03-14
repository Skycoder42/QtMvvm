#ifndef QTMVVM_INJECTION_H
#define QTMVVM_INJECTION_H

#include "qtmvvmcore_global.h"

//! The primary namespace of the QtMvvm library
namespace QtMvvm {

//! Registers QVariant converters from QObject to an interface type registered with Q_DECLARE_INTERFACE
template <typename TInterface>
inline void registerInterfaceConverter() {
	QMetaType::registerConverter<QObject*, TInterface*>([](QObject *o) {
		return qobject_cast<TInterface*>(o);
	});
}

}

//! Mark a property for injection
#define QTMVVM_INJECT(classType, name) \
	static inline QByteArray __qtmvvm_inject_##name() { \
		return QtMvvm::__helpertypes::inject_iid<classType>(); \
	} \
	Q_PROPERTY(QByteArray __qtmvvm_inject_##name READ __qtmvvm_inject_##name STORED false SCRIPTABLE false DESIGNABLE false CONSTANT FINAL)

//! Create a for injection marked property based on a member
#define QTMVVM_INJECT_PROP(type, name, member) \
	Q_PROPERTY(type name MEMBER member) \
	QTMVVM_INJECT(type, name)

//! @file injection.h A header with injection related code
#endif // INJECTION_H

#ifndef QTMVVMCORE_GLOBAL_H
#define QTMVVMCORE_GLOBAL_H

#include <QtCore/qglobal.h>

#include "qtmvvmcore_helpertypes.h"

#if defined(QT_BUILD_MVVMCORE_LIB)
#	define Q_MVVMCORE_EXPORT Q_DECL_EXPORT
#else
#	define Q_MVVMCORE_EXPORT Q_DECL_IMPORT
#endif

#define QTMVVM_INJECT(classType, name) \
	static inline QByteArray __qtmvvm_inject_##name() { \
		return QtMvvm::__helpertypes::inject_iid<classType>(); \
	} \
	Q_PROPERTY(QByteArray __qtmvvm_inject_##name READ __qtmvvm_inject_##name STORED false SCRIPTABLE false DESIGNABLE false CONSTANT FINAL)

#define QTMVVM_INJECT_PROP(type, name, member) \
	Q_PROPERTY(type name MEMBER member) \
	QTMVVM_INJECT(type, name)

#endif // QTMVVMCORE_GLOBAL_H

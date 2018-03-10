#ifndef QTMVVMCORE_GLOBAL_H
#define QTMVVMCORE_GLOBAL_H

#include <QtCore/qglobal.h>

#include "qtmvvmcore_helpertypes.h"

#if defined(QT_BUILD_MVVMCORE_LIB)
#	define Q_MVVMCORE_EXPORT Q_DECL_EXPORT
#else
#	define Q_MVVMCORE_EXPORT Q_DECL_IMPORT
#endif

#endif // QTMVVMCORE_GLOBAL_H

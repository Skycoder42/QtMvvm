#ifndef QTMVVMQUICK_GLOBAL_H
#define QTMVVMQUICK_GLOBAL_H

#include <QtCore/qglobal.h>

#ifndef QT_STATIC
#  if defined(QT_BUILD_MVVMQUICK_LIB)
#    define Q_MVVMQUICK_EXPORT Q_DECL_EXPORT
#  else
#    define Q_MVVMQUICK_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define Q_MVVMQUICK_EXPORT
#endif

#endif // QTMVVMQUICK_GLOBAL_H

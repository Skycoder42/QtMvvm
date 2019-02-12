#ifndef QTMVVMWIDGETS_GLOBAL_H
#define QTMVVMWIDGETS_GLOBAL_H

#include <QtCore/qglobal.h>

#ifndef QT_STATIC
#  if defined(QT_BUILD_MVVMWIDGETS_LIB)
#    define Q_MVVMWIDGETS_EXPORT Q_DECL_EXPORT
#  else
#    define Q_MVVMWIDGETS_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define Q_MVVMWIDGETS_EXPORT
#endif

#endif // QTMVVMWIDGETS_GLOBAL_H

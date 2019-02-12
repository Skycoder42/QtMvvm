#ifndef MVVMDATASYNCWIDGETS_GLOBAL_H
#define MVVMDATASYNCWIDGETS_GLOBAL_H

#include <QtCore/qglobal.h>

#ifndef QT_STATIC
#  if defined(QT_BUILD_MVVMDATASYNCWIDGETS_LIB)
#    define Q_MVVMDATASYNCWIDGETS_EXPORT Q_DECL_EXPORT
#  else
#    define Q_MVVMDATASYNCWIDGETS_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define Q_MVVMDATASYNCWIDGETS_EXPORT
#endif

namespace QtMvvm {

//! A method to initialize the datasync widgets mvvm module
Q_MVVMDATASYNCWIDGETS_EXPORT void registerDataSyncWidgets();

}

#endif // MVVMDATASYNCWIDGETS_GLOBAL_H

#include "qtmvvmcore_global.h"
#include "qtmvvm_logging_p.h"

namespace QtMvvm {

#ifdef QT_NO_DEBUG
Q_LOGGING_CATEGORY(mvvmcore, "qtmvvm", QtInfoMsg)
#else
Q_LOGGING_CATEGORY(mvvmcore, "qtmvvm", QtDebugMsg)
#endif

}

#ifndef QTMVVM_LOGGING_P_H
#define QTMVVM_LOGGING_P_H

#include <QtCore/QLoggingCategory>

#include "qtmvvmcore_global.h"

namespace QtMvvm {

Q_MVVMCORE_EXPORT Q_DECLARE_LOGGING_CATEGORY(mvvmLoggingCat)

}

#define logDebug(...) qCDebug(mvvmLoggingCat, __VA_ARGS__)
#define logInfo(...) qCInfo(mvvmLoggingCat, __VA_ARGS__)
#define logWarning(...) qCWarning(mvvmLoggingCat, __VA_ARGS__)
#define logCritical(...) qCCritical(mvvmLoggingCat, __VA_ARGS__)

#endif // QTMVVM_LOGGING_P_H

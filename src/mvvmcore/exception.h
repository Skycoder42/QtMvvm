#ifndef QTMVVM_EXCEPTION_H
#define QTMVVM_EXCEPTION_H

#include "QtMvvmCore/qtmvvmcore_global.h"

#if !defined(QT_NO_EXCEPTIONS) && QT_CONFIG(future)
#include <QtCore/QException>
#define QTMVVM_EXCEPTION_BASE QException
#define QTMVVM_EXCEPTION_OR override
#else
#include <exception>
#define QTMVVM_EXCEPTION_BASE std::exception
#define QTMVVM_EXCEPTION_OR
#endif

#endif // QTMVVM_EXCEPTION_H

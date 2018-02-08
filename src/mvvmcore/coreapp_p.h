#ifndef QTMVVM_COREAPP_P_H
#define QTMVVM_COREAPP_P_H

#include <QtCore/QPointer>

#include "qtmvvmcore_global.h"
#include "coreapp.h"

namespace QtMvvm {

class CoreAppPrivate
{
public:
	CoreAppPrivate();

	static bool bootEnabled;
	static QPointer<CoreApp> instance;
};

}

#endif // QTMVVM_COREAPP_P_H

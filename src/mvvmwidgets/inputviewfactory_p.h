#ifndef QTMVVM_INPUTVIEWFACTORY_P_H
#define QTMVVM_INPUTVIEWFACTORY_P_H

#include "qtmvvmwidgets_global.h"
#include "inputviewfactory.h"

namespace QtMvvm {

class InputViewFactoryPrivate
{
public:
	InputViewFactoryPrivate();

	QHash<QByteArray, std::function<QWidget*(QWidget*)>> simpleWidgets;
};

}

#endif // QTMVVM_INPUTVIEWFACTORY_P_H

#ifndef QTMVVM_INPUTWIDGETFACTORY_P_H
#define QTMVVM_INPUTWIDGETFACTORY_P_H

#include "qtmvvmwidgets_global.h"
#include "inputwidgetfactory.h"

namespace QtMvvm {

class InputWidgetFactoryPrivate
{
	Q_DISABLE_COPY(InputWidgetFactoryPrivate)

public:
	InputWidgetFactoryPrivate() = default;

	QHash<QByteArray, std::function<QWidget*(QWidget*)>> simpleWidgets;
	QHash<QByteArray, QByteArray> aliases;
};

}

#endif // QTMVVM_INPUTWIDGETFACTORY_P_H

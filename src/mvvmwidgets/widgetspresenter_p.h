#ifndef QTMVVM_WIDGETSPRESENTER_P_H
#define QTMVVM_WIDGETSPRESENTER_P_H

#include <QtCore/qset.h>
#include <QtCore/qhash.h>

#include "qtmvvmwidgets_global.h"
#include "widgetspresenter.h"

namespace QtMvvm {

class WidgetsPresenterPrivate
{
public:
	WidgetsPresenterPrivate();

	static WidgetsPresenter *currentPresenter();

	QSet<const QMetaObject*> implicitMappings;
	QHash<QByteArray, const QMetaObject*> explicitMappings;
};

}

#endif // QTMVVM_WIDGETSPRESENTER_P_H

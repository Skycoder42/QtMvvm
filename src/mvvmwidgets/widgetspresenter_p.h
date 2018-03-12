#ifndef QTMVVM_WIDGETSPRESENTER_P_H
#define QTMVVM_WIDGETSPRESENTER_P_H

#include <QtCore/QSet>
#include <QtCore/QHash>
#include <QtGui/QValidator>

#include "qtmvvmwidgets_global.h"
#include "widgetspresenter.h"

namespace QtMvvm {

class WidgetsPresenterPrivate
{
public:
	WidgetsPresenterPrivate();

	static WidgetsPresenter *currentPresenter();

	InputWidgetFactory* inputViewFactory;
	QSet<const QMetaObject*> implicitMappings;
	QHash<const QMetaObject*, const QMetaObject*> explicitMappings;
};

Q_MVVMWIDGETS_EXPORT QValidator *createUrlValidator(const QStringList &schemes, QObject* parent = nullptr);

}

#endif // QTMVVM_WIDGETSPRESENTER_P_H

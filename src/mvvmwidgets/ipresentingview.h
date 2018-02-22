#ifndef QTMVVM_IPRESENTINGVIEW_H
#define QTMVVM_IPRESENTINGVIEW_H

#include <QtWidgets/qwidget.h>

#include "QtMvvmWidgets/qtmvvmwidgets_global.h"

namespace QtMvvm {

class Q_MVVMWIDGETS_EXPORT IPresentingView
{
public:
	inline virtual ~IPresentingView() = default;

	virtual bool tryPresent(QWidget *view) = 0;
};

}

#define QtMvvm_IPresentingViewIid "de.skycoder42.qtmvvm.widgets.IPresentingView"
Q_DECLARE_INTERFACE(QtMvvm::IPresentingView, QtMvvm_IPresentingViewIid)

#endif // QTMVVM_IPRESENTINGVIEW_H

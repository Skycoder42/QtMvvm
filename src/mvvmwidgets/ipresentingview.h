#ifndef QTMVVM_IPRESENTINGVIEW_H
#define QTMVVM_IPRESENTINGVIEW_H

#include <QtWidgets/qwidget.h>

#include "QtMvvmWidgets/qtmvvmwidgets_global.h"

namespace QtMvvm {

//! A simple interface to make it possible for any view to present subviews
class Q_MVVMWIDGETS_EXPORT IPresentingView
{
	Q_DISABLE_COPY(IPresentingView)
public:
	inline IPresentingView() = default;
	inline virtual ~IPresentingView() = default;

	//! Is called to try to present a view
	virtual bool tryPresent(QWidget *view) = 0;
};

}

//! The IID of the QtMvvm::IPresentingView class
#define QtMvvm_IPresentingViewIid "de.skycoder42.qtmvvm.widgets.IPresentingView"
Q_DECLARE_INTERFACE(QtMvvm::IPresentingView, QtMvvm_IPresentingViewIid)

//! @file ipresentingview.h The header of the IPresentingView interface
#endif // QTMVVM_IPRESENTINGVIEW_H

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

#endif // QTMVVM_IPRESENTINGVIEW_H

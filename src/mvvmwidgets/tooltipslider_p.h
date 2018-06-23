#ifndef QTMVVM_TOOLTIPSLIDER_P_H
#define QTMVVM_TOOLTIPSLIDER_P_H

#include <QtWidgets/QSlider>

#include "qtmvvmwidgets_global.h"

namespace QtMvvm {

class ToolTipSlider : public QSlider
{
	Q_OBJECT

public:
	explicit ToolTipSlider(QWidget *parent = nullptr);

private Q_SLOTS:
	void onMoved(int position);
};

}

#endif // QTMVVM_TOOLTIPSLIDER_P_H

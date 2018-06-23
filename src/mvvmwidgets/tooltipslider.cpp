#include "tooltipslider_p.h"
#include <QtCore/QVariant>
#include <QtWidgets/QToolTip>
using namespace QtMvvm;

ToolTipSlider::ToolTipSlider(QWidget *parent) :
	QSlider{Qt::Horizontal, parent}
{
	setTracking(false);
	connect(this, &ToolTipSlider::sliderMoved,
			this, &ToolTipSlider::onMoved);
}

void ToolTipSlider::onMoved(int position)
{
	auto format = property("valueFormat").toString();
	if(format.isEmpty())
		format = QStringLiteral("%L1");
	auto mPos = mapToGlobal({0,0});
	mPos.setX(QCursor::pos().x());
	QToolTip::showText(mPos, format.arg(position), this);
}

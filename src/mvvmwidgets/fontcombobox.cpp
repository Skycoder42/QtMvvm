#include "fontcombobox_p.h"
using namespace QtMvvm;

FontComboBox::FontComboBox(QWidget *parent) :
	QFontComboBox(parent)
{
	connect(this, &FontComboBox::currentFontChangedImp,
			this, &FontComboBox::currentFontChanged);
}

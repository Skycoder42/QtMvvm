#include "fontcombobox_p.h"
using namespace QtMvvm;

FontComboBox::FontComboBox(QWidget *parent) :
	QFontComboBox(parent)
{
	connect(this, &FontComboBox::currentFontChanged,
			this, &FontComboBox::currentFontChangedImp);
	setCurrentFont(font());
}

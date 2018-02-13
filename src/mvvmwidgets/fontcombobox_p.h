#ifndef QTMVVM_FONTCOMBOBOX_P_H
#define QTMVVM_FONTCOMBOBOX_P_H

#include <QtWidgets/QFontComboBox>

#include "qtmvvmwidgets_global.h"

namespace QtMvvm {

class FontComboBox : public QFontComboBox
{
	Q_OBJECT

	Q_PROPERTY(QFont currentFont READ currentFont WRITE setCurrentFont NOTIFY currentFontChangedImp USER true)

public:
	explicit FontComboBox(QWidget *parent = nullptr);

Q_SIGNALS:
	void currentFontChangedImp(const QFont &font);
};

}

#endif // QTMVVM_FONTCOMBOBOX_P_H

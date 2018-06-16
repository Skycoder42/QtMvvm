#ifndef QTMVVM_SELECTCOMBOBOX_P_H
#define QTMVVM_SELECTCOMBOBOX_P_H

#include <QtCore/QVariant>

#include <QtWidgets/QComboBox>

#include "qtmvvmwidgets_global.h"

namespace QtMvvm {

class SelectComboBox : public QComboBox
{
	Q_OBJECT

	Q_PROPERTY(QVariant currentValue READ currentValue WRITE setCurrentValue NOTIFY currentValueChanged USER true)
	Q_PROPERTY(QVariantList listElements READ listElements WRITE setListElements NOTIFY listElementsChanged)

public:
	explicit SelectComboBox(QWidget *parent = nullptr);

	QVariant currentValue() const;
	QVariantList listElements() const;

public Q_SLOTS:
	void setCurrentValue(const QVariant &data);
	void setListElements(const QVariantList &listElements);

Q_SIGNALS:
	void currentValueChanged();
	void listElementsChanged(const QVariantList &listElements);
};

}

#endif // QTMVVM_SELECTCOMBOBOX_P_H

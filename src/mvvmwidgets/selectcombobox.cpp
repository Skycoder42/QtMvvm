#include "selectcombobox_p.h"
using namespace QtMvvm;

SelectComboBox::SelectComboBox(QWidget *parent) :
	QComboBox(parent)
{
	connect(this, &SelectComboBox::currentTextChanged,
			this, &SelectComboBox::currentValueChanged);
	connect(this, &SelectComboBox::editTextChanged,
			this, &SelectComboBox::currentValueChanged);
}

QVariant SelectComboBox::currentValue() const
{
	if(currentText() != itemText(currentIndex()))
		return currentText();
	else
		return currentData();
}

QVariantList SelectComboBox::listElements() const
{
	QVariantList res;
	auto mCount = count();
	res.reserve(mCount);
	for(auto i = 0; i < mCount; i++) {
		auto key = itemText(i);
		auto value = itemData(i);
		if(key == value)
			res.append(key);
		else {
			QVariantMap map;
			map.insert(QStringLiteral("name"), key);
			map.insert(QStringLiteral("value"), value);
			res.append(map);
		}
	}
	return res;
}

void SelectComboBox::setCurrentValue(const QVariant &data)
{
	auto index = findData(data);
	if(index != -1)
		setCurrentIndex(index);
	else if(isEditable())
		setCurrentText(data.toString());
}

void SelectComboBox::setListElements(const QVariantList &listElements)
{
	clear();
	for(const auto &item : listElements) {
		if(item.type() == QVariant::String)
			addItem(item.toString(), item);
		else {
			auto iData = item.toMap();
			addItem(iData.value(QStringLiteral("name")).toString(),
					iData.value(QStringLiteral("value")));
		}
	}
	emit listElementsChanged(listElements);
}


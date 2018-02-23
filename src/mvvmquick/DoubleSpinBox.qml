import QtQuick 2.10
import QtQuick.Controls 2.3

SpinBox {
	id: _edit
	property alias inputValue: _edit.dValue
	property alias minimum: _edit.dFrom
	property alias maximum: _edit.dTo
	editable: true

	//double spinbox code
	property int decimals: 2
	property double dFrom: 0.0
	property double dTo: 100.0
	property double dValue: 0.0
	property double dStepSize: 0.1

	readonly property int factor: Math.pow(10, _edit.decimals)

	stepSize: _edit.dStepSize * _edit.factor
	from: _edit.dFrom * _edit.factor
	to: _edit.dTo * _edit.factor
	value: _edit.dValue * _edit.factor

	validator: DoubleValidator {
		bottom: _edit.dFrom
		top: _edit.dTo
	}

	textFromValue: function(value, locale) {
		return Number(value / _edit.factor).toLocaleString(locale, 'f', _edit.decimals);
	}

	valueFromText: function(text, locale) {
		return Number.fromLocaleString(locale, text) * _edit.factor;
	}

	onDValueChanged: _edit.value = _edit.dValue * _edit.factor
	onValueChanged: _edit.dValue = _edit.value / _edit.factor
}

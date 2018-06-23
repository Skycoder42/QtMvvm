import QtQuick 2.10
import QtQuick.Controls 2.3

Slider {
	id: _edit

	property int inputValue
	property alias minimum: _edit.from
	property alias maximum: _edit.to
	property string valueFormat: "%L1"

	stepSize: 1
	snapMode: Slider.SnapAlways
	live: false
	value: inputValue
	onValueChanged: {
		var nValue = Math.round(_edit.value)
		if(nValue != inputValue)
			inputValue = nValue
	}

	ToolTip {
		parent: _slider.handle
		visible: _slider.pressed
		text: valueFormat.arg(Math.round(_slider.from + _slider.visualPosition * (_slider.to - _slider.from)))
	}
}

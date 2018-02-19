import QtQuick 2.10
import QtQuick.Controls 2.3

ComboBox {
	id: _edit
	property alias inputValue: _edit.currentValue
	property var currentValue: getCurrentValue()
	property alias listElements: _edit.model

	textRole: "name"

	function getCurrentValue() {
		var value = _edit.model[_edit.currentIndex].value;
		if(typeof value !== "undefined")
			return value;
		else
			return _edit.displayText;
	}
}

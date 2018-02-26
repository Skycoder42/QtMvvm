import QtQuick 2.10
import QtQuick.Controls 2.3

ComboBox {
	id: _edit
	property var inputValue
	property alias listElements: _edit.model
	readonly property bool isExtended: Boolean(model[0] && model[0].name)

	property bool _skipNext: false

	textRole: isExtended ? "name" : ""

	onInputValueChanged: {
		if(_skipNext) {
			_skipNext = false;
			return;
		} else
			_skipNext = true;

		var found = false;
		var i;
		if(isExtended) {
			for(i = 0; i < model.length; i++) {
				if(model[i].value == inputValue) {
					currentIndex = i;
					found = true;
					break;
				}
			}
		} else {
			for(i = 0; i < model.length; i++) {
				if(model[i] == inputValue) {
					currentIndex = i;
					found = true;
					break;
				}
			}
		}
		if(!found)
			editText = inputValue;
	}

	onEditTextChanged: {
		if(_skipNext) {
			_skipNext = false;
			return;
		} else
			_skipNext = true;

		if(isExtended) {
			var value = model[currentIndex].value;
			if(typeof value !== "undefined")
				inputValue = value;
			else
				inputValue = editText;
		} else
			inputValue = editText
	}
}

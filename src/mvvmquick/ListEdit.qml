import QtQuick 2.10
import QtQuick.Controls 2.3

ComboBox {
	id: _edit
	property alias inputValue: _edit.currentValue
	property alias listElements: _edit.model
	property var currentValue: _valueHelper ? _valueHelper.value : null

	textRole: "name"
	property var _valueHelper: { return {}; }

	Component.onCompleted: {
		Object.defineProperty(
			_valueHelper,
			"value",
			{
				get: function () {
					var value = _edit.model[_edit.currentIndex].value;
					if(typeof value !== "undefined")
						return value;
					else
						return _edit.displayText;
				},
				set: function (value) {
					var index = find(value); //TODO find VALUE, not text... somehow via the model?
					if(index !== -1)
						currentIndex = index;
					else if(editable)
						editText = value;
				}
			}
		);
	}
}

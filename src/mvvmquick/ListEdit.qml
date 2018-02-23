import QtQuick 2.10
import QtQuick.Controls 2.3

ComboBox {
	id: _edit
	property alias inputValue: _edit.currentValue
	property alias listElements: _edit.model
	property alias currentValue: _valueHelper.value

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
					var index = find(value);
					if(index !== -1)
						currentIndex = index;
					else if(editable)
						editText = value;
				}
			}
		);
	}
}

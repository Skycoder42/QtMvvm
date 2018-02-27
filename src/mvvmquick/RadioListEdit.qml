import QtQuick 2.10
import QtQuick.Controls 2.3

ListView {
	id: _edit
	property var inputValue
	property alias listElements: _edit.model
	readonly property bool isExtended: Boolean(model[0] && model[0].name)

	implicitHeight: dummyDelegate.height * model.length

	ScrollBar.vertical: ScrollBar {}

	delegate: RadioDelegate {
		width: parent.width
		text: isExtended ? _edit.model[index].name : modelData

		checked: (isExtended ? _edit.model[index].value : modelData) == inputValue
		onClicked: inputValue = (isExtended ? _edit.model[index].value : modelData)
	}

	RadioDelegate {
		id:dummyDelegate
		visible: false
		text: "dummy"
	}
}

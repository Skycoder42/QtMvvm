import QtQuick 2.10
import QtQuick.Controls 2.3

Item {
	id: _edit
	implicitWidth: _listEdit.implicitWidth
	implicitHeight: _listEdit.implicitHeight

	property font inputValue: _listEdit.font

	ListEdit {
		id: _listEdit
		anchors.fill: parent

		inputValue: _edit.inputValue.family
		onInputValueChanged: _edit.inputValue = Qt.font({family: inputValue})

		model: Qt.fontFamilies()
		editable: false
	}
}

import QtQuick 2.10
import QtQuick.Controls 2.3

ComboBox {
	id: _edit
	property alias inputValue: _edit.currentText

	model: Qt.fontFamilies()
	editable: false
}

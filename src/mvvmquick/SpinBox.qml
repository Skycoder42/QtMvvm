import QtQuick 2.10
import QtQuick.Controls 2.3

SpinBox {
	id: _edit
	property alias inputValue: _edit.value
	property alias minimum: _edit.from
	property alias maximum: _edit.to
	editable: true
}

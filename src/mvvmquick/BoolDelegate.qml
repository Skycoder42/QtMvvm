import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

CheckDelegate {
	id: _boolDelegate

	text: title

	Component.onCompleted: checked = settingsValue; //TODO buggy, use binding?
	onCheckedChanged: settingsValue = checked;

	contentItem: GridLayout {
		columns: 2
		rows: 2

		Label {
			id: _titleLabel
			text: _boolDelegate.text
			Layout.row: 0
			Layout.column: 0
			font.bold: true
			elide: Label.ElideRight
			Layout.fillWidth: true
		}

		Label {
			id: _textLabel
			visible: tooltip
			Layout.row: 1
			Layout.column: 0
			text: tooltip
			wrapMode: Text.WordWrap
			Layout.fillWidth: true
		}

		Item {
			Layout.row: 0
			Layout.column: 1
			Layout.rowSpan: 2
			Layout.minimumWidth: implicitWidth
			Layout.maximumWidth: implicitWidth
			implicitWidth: _boolDelegate.indicator.width + 14
		}
	}
}

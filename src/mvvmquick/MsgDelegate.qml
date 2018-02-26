import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

ItemDelegate {
	id: _msgDelegate

	text: title

	signal showInput(string key, string title, string type, var properties);

	contentItem: ColumnLayout {
		Label {
			id: _titleLabel
			text: _msgDelegate.text
			font.bold: true
			elide: Label.ElideRight
			Layout.fillWidth: true
		}

		Label {
			id: _textLabel
			visible: tooltip
			text: tooltip
			wrapMode: Text.WordWrap
			Layout.fillWidth: true
		}
	}

	onClicked: showInput(key, title, type, properties)
}

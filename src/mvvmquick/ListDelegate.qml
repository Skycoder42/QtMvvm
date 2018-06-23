import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Quick 1.1

ItemDelegate {
	id: _listDelegate

	text: title

	signal showInput(string key, string title, string type, var defaultValue, var properties);

	ToolTip.visible: pressed && tooltip != ""
	ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
	ToolTip.text: tooltip
	onPressAndHold: QuickPresenter.hapticLongPress()

	contentItem: ColumnLayout {
		Label {
			id: _titleLabel
			text: _listDelegate.text
			font.bold: true
			elide: Label.ElideRight
			Layout.fillWidth: true
		}

		Label {
			id: _previewLabel
			visible: preview
			text: preview
			wrapMode: Text.WordWrap
			Layout.fillWidth: true
		}
	}

	onClicked: showInput(key, title, "radiolist", inputValue, properties)
}

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Quick 1.1

ItemDelegate {
	id: _msgDelegate

	property alias indicatorComponent: _indicator.sourceComponent
	property alias indicatorSource: _indicator.source

	property string editDialogType: type

	signal showInput(string key, string title, string type, var defaultValue, var properties);

	text: title

	ToolTip.visible: pressed && tooltip != ""
	ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
	ToolTip.text: tooltip
	onPressAndHold: QuickPresenter.hapticLongPress()

	onClicked: showInput(key, title, editDialogType, inputValue, properties)

	contentItem: GridLayout {
		columns: 2
		rows: 2

		Label {
			id: _titleLabel
			text: _msgDelegate.text
			Layout.row: 0
			Layout.column: 0
			font.bold: true
			elide: Label.ElideRight
			Layout.fillWidth: true
		}

		Label {
			id: _peviewLabel
			visible: preview
			Layout.row: 1
			Layout.column: 0
			text: preview
			wrapMode: Text.WordWrap
			Layout.fillWidth: true
		}

		Loader {
			id: _indicator
			visible: item
			asynchronous: false
			clip: true

			Layout.row: 0
			Layout.column: 1
			Layout.rowSpan: 2
			Layout.minimumWidth: item ? implicitWidth : 0
			Layout.maximumWidth: item ? implicitWidth : 0
		}
	}
}

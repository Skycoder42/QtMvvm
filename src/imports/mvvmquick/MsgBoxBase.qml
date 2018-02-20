import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Core 1.0

Dialog {
	id: _msgBoxBase

	property var msgConfig
	property MessageResult msgResult

	property real extraHeight: 0
	property real baseWidth: 300

	property alias iconVisible: _icon.visible
	property alias iconSource: _icon.source

	x: parent ? (parent.width - width) / 2 : 0
	y: parent ? deltaY() : 0
	width: parent ? Math.min(Math.max(implicitWidth, baseWidth), parent.width - 28) : implicitWidth
	height: parent ? Math.min(implicitHeight, parent.height - 28) : implicitWidth
	modal: true
	focus: true

	function deltaY() {
		var unscaled = Qt.inputMethod.keyboardRectangle.height / Screen.devicePixelRatio;
		var availHeight = (parent.height + extraHeight) - unscaled - 28;//spacing
		var rawDelta = (Math.max(0, availHeight - height) / 2);
		return rawDelta + 14 - extraHeight;//spacing
	}

	header: RowLayout {
		spacing: 14

		TintIcon {
			id: _icon
			visible: false

			Layout.preferredWidth: 24
			Layout.preferredHeight: 24
			Layout.margins: 24
			Layout.bottomMargin: 0
			Layout.rightMargin: 0
		}

		Label {
			id: _title
			text: msgConfig.title
			visible: msgConfig.title
			elide: Label.ElideRight
			font.bold: true
			font.pixelSize: 16
			Layout.fillWidth: true
			Layout.margins: 24
			Layout.bottomMargin: 0
			Layout.leftMargin: _icon.visible ? 0 : 24
		}
	}

	footer: DialogButtonBox {
		id: _btnBox

		readonly property var _allBtns: [
			DialogButtonBox.NoButton,
			DialogButtonBox.Ok,
			DialogButtonBox.Save,
			DialogButtonBox.SaveAll,
			DialogButtonBox.Open,
			DialogButtonBox.Yes,
			DialogButtonBox.YesToAll,
			DialogButtonBox.No,
			DialogButtonBox.NoToAll,
			DialogButtonBox.Abort,
			DialogButtonBox.Retry,
			DialogButtonBox.Ignore,
			DialogButtonBox.Close,
			DialogButtonBox.Cancel,
			DialogButtonBox.Discard,
			DialogButtonBox.Help,
			DialogButtonBox.Apply,
			DialogButtonBox.Reset,
			DialogButtonBox.RestoreDefaults,
		]

		standardButtons: msgConfig.buttons
		onStandardButtonsChanged: {
			for(var key in msgConfig.buttonTexts)
				standardButton(DialogButtonBox.Ok).text = msgConfig.buttonTexts[key]
		}

		onClicked: {
			if(msgResult) {
				_allBtns.forEach(function(sBtn) {
					if(button === standardButton(sBtn)) {
						msgResult.complete(sBtn);
						msgResult = null;
					}
				})
			}
		}
	}

	onClosed: {
		if(msgResult) {
			msgResult.complete(MessageConfig.NoButton);
			msgResult = null;
		}
	}
}

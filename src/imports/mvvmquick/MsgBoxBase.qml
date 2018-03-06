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
	width: parent ? Math.min(Math.max(implicitWidth, baseWidth), parent.width - 24) : implicitWidth
	height: parent ? Math.min(implicitHeight, parent.height - 24) : implicitWidth
	modal: true
	focus: true

	function deltaY() {
		var unscaled = Qt.inputMethod.keyboardRectangle.height / Screen.devicePixelRatio;
		var availHeight = (parent.height + extraHeight) - unscaled - 24; //margins
		var rawDelta = (Math.max(0, availHeight - height) / 2);
		return rawDelta + 12 - extraHeight; //spacing
	}

	header: Pane {
		id: _headerPane
		background: Item {}
		bottomPadding: 0

		RowLayout {
			anchors.fill: parent
			spacing: _headerPane.padding

			TintIcon {
				id: _icon
				visible: false

				Layout.preferredWidth: 24
				Layout.preferredHeight: 24
				Layout.alignment: Qt.AlignVCenter
			}

			Label {
				id: _title
				text: msgConfig.title
				visible: msgConfig.title
				elide: Label.ElideRight
				font.bold: true
				Layout.fillWidth: true
			}
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
				standardButton(key).text = msgConfig.buttonTexts[key]
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

	Component.onCompleted: {
		if(msgResult)
			msgResult.setCloseTarget(_msgBoxBase, "reject()");
	}
}

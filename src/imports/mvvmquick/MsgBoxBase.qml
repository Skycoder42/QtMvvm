import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1

AlertDialog {
	id: _msgBoxBase

	property var msgConfig
	property MessageResult msgResult

	property alias iconVisible: _icon.visible
	property alias iconSource: _icon.icon.source
	property bool autoHandleBtns: true

	signal buttonClicked(int button)

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

				Layout.preferredWidth: implicitWidth
				Layout.preferredHeight: implicitHeight
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

	footer: Item {
		visible: msgConfig.buttons !== MessageConfig.NoButton
		implicitWidth: _btnBox.implicitWidth
		implicitHeight: _btnBox.implicitHeight

		DialogButtonBox {
			id: _btnBox
			anchors.fill: parent

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
				Object.keys(msgConfig.buttonTexts).forEach(function(key) {
					standardButton(key).text = msgConfig.buttonTexts[key]
				});
			}

			onClicked: {
				_msgBoxBase.buttonClicked(button);
				if(msgResult && autoHandleBtns) {
					_allBtns.forEach(function(sBtn) {
						if(button == standardButton(sBtn)) {
							msgResult.complete(sBtn);
							msgResult = null;
							_msgBoxBase.done(sBtn);
						}
					});
				}
			}
		}
	}

	onClosed: {
		if(msgResult && autoHandleBtns) {
			msgResult.complete(MessageConfig.NoButton);
			msgResult = null;
		}
	}

	Component.onCompleted: {
		if(msgResult && autoHandleBtns)
			msgResult.setCloseTarget(_msgBoxBase, "reject()");
	}
}

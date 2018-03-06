import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import de.skycoder42.QtMvvm.Core 1.0
import de.skycoder42.QtMvvm.Quick 1.0
import de.skycoder42.QtMvvm.DataSync.Core 1.0

Dialog {
	id: _exportSetupView

	property PExportSetupViewModel viewModel: null

	property real extraHeight: 0
	property real baseWidth: 300

	x: parent ? (parent.width - width) / 2 : 0
	y: parent ? deltaY() : 0
	width: parent ? Math.min(Math.max(implicitWidth, baseWidth), parent.width - 24) : implicitWidth
	height: parent ? Math.min(implicitHeight, parent.height - 24) : implicitWidth
	modal: true
	focus: true

	function deltaY() { //TODO move to common js file?
		var unscaled = Qt.inputMethod.keyboardRectangle.height / Screen.devicePixelRatio;
		var availHeight = (parent.height + extraHeight) - unscaled - 24; //margins
		var rawDelta = (Math.max(0, availHeight - height) / 2);
		return rawDelta + 12 - extraHeight; //spacing
	}

	title: viewModel.label

	ColumnLayout {
		id: _layout
		anchors.fill: parent

		CheckBox {
			id: _trustBox
			text: qsTr("Trusted")
			Layout.fillWidth: true

			MvvmBinding {
				viewModel: _exportSetupView.viewModel
				viewModelProperty: "trusted"
				view: _trustBox
				viewProperty: "checked"
			}
		}

		CheckBox {
			id: _includeBox
			text: qsTr("Include Server")
			Layout.fillWidth: true

			MvvmBinding {
				viewModel: _exportSetupView.viewModel
				viewModelProperty: "includeServer"
				view: _includeBox
				viewProperty: "checked"
			}
		}

		Label {
			text: qsTr("Password:")
			Layout.fillWidth: true
			color: _passwordEdit.focus ? _passwordEdit.selectionColor : palette.text
			opacity: _passwordEdit.focus ? 1 : 0.5
			enabled: viewModel.trusted
		}

		TextField {
			id: _passwordEdit
			echoMode: TextInput.Password
			Layout.fillWidth: true
			enabled: viewModel.trusted

			MvvmBinding {
				viewModel: _exportSetupView.viewModel
				viewModelProperty: "password"
				view: _passwordEdit
				viewProperty: "text"
				type: MvvmBinding.OneWayToViewModel
			}
		}
	}

	standardButtons: Dialog.Ok | Dialog.Cancel

	onAccepted: viewModel.completeSetup()

	Component.onCompleted: standardButton(Dialog.Ok).enabled = Qt.binding(function(){ return viewModel.valid; })
}

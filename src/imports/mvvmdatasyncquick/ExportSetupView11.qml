import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1
import de.skycoder42.QtMvvm.DataSync.Core 1.1

AlertDialog {
	id: _exportSetupView

	property PExportSetupViewModel viewModel: null

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

		DecorLabel {
			text: qsTr("Password:")
			Layout.fillWidth: true
			edit: _passwordEdit
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

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1
import de.skycoder42.QtMvvm.DataSync.Core 1.1

AlertDialog {
	id: _identityEditView

	property PIdentityEditViewModel viewModel: null

	title: qsTr("Edit Identity")

	ColumnLayout {
		id: _layout
		anchors.fill: parent

		DecorLabel {
			text: qsTr("Device Name:")
			Layout.fillWidth: true
			edit: _nameEdit
		}

		TextField {
			id: _nameEdit
			Layout.fillWidth: true

			MvvmBinding {
				viewModel: _identityEditView.viewModel
				viewModelProperty: "name"
				view: _nameEdit
				viewProperty: "text"
			}
		}

		DecorLabel {
			id: _fpLabel
			text: qsTr("Device Fingerprint:")
			Layout.fillWidth: true
			Layout.topMargin: 16
			edit: _fpText
		}

		Label {
			id: _fpText
			text: viewModel.fingerPrint
			Layout.fillWidth: true
			wrapMode: Text.Wrap
			verticalAlignment: Qt.AlignVCenter
			font.pointSize: _fpLabel.font.pointSize * 0.8
			Layout.topMargin: 8
		}
	}

	standardButtons: Dialog.Save | Dialog.Cancel

	onAccepted: viewModel.save()

	Component.onCompleted: standardButton(Dialog.Save).enabled = Qt.binding(function(){ return viewModel.valid; })
}

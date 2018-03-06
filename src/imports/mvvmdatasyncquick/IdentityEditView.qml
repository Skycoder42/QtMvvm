import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import de.skycoder42.QtMvvm.Core 1.0
import de.skycoder42.QtMvvm.Quick 1.0
import de.skycoder42.QtMvvm.DataSync.Core 1.0

AlertDialog {
	id: _identityEditView

	property PIdentityEditViewModel viewModel: null

	title: qsTr("Edit Identity")

	ColumnLayout {
		id: _layout
		anchors.fill: parent

		Label {
			text: qsTr("Device Name:")
			Layout.fillWidth: true
			color: _nameEdit.focus ? _nameEdit.selectionColor : palette.text
			opacity: _nameEdit.focus ? 1 : 0.5
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

		Label {
			id: _fpLabel
			text: qsTr("Device Fingerprint:")
			Layout.fillWidth: true
			opacity: 0.5
			Layout.topMargin: 16
		}

		Label {
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

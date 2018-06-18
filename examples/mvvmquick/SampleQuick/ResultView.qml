import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1
import de.skycoder42.QtMvvm.Sample 1.1

AlertDialog {
	id: resultDialog

	property ResultViewModel viewModel: null

	title: qsTr("Enter something")

	ColumnLayout {
		anchors.fill: parent

		Label {
			text: qsTr("Enter a result to be reported as event to the main view:")
			wrapMode: Text.WordWrap
			Layout.fillWidth: true
		}

		TextField {
			id: resultEdit
			Layout.fillWidth: true
			selectByMouse: true

			MvvmBinding {
				viewModel: resultDialog.viewModel
				viewModelProperty: "result"
				view: resultEdit
				viewProperty: "text"
			}
		}
	}

	standardButtons: Dialog.Ok | Dialog.Cancel

	onAccepted: viewModel.done()
}

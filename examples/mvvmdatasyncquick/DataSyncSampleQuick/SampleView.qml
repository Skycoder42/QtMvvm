import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Core 1.0
import de.skycoder42.QtMvvm.Quick 1.0
import de.skycoder42.QtMvvm.Sample 1.0

Page {
	id: sampleView
	property SampleViewModel viewModel: null
	readonly property bool presentAsRoot: true

	header: ToolBar {
		height: 56

		Label {
			anchors.fill: parent
			font.pointSize: 16
			font.bold: true
			elide: Label.ElideRight
			horizontalAlignment: Qt.AlignLeft
			verticalAlignment: Qt.AlignVCenter
			leftPadding: 10
			text: qsTr("Datasync Sample")
		}
	}

	PresenterProgress {}

	Pane {
		anchors.fill: parent
		GridLayout {
			id: grid
			anchors.fill: parent

			columns: 2

			ListView {
				id: mainList
				Layout.columnSpan: 2
				Layout.fillHeight: true
				Layout.fillWidth: true
				clip: true
				model: viewModel.model

				delegate: ItemDelegate {
					width: parent.width
					text: key
					onPressAndHold: viewModel.removeAt(index)
				}
			}

			TextField {
				id: addField
				placeholderText: "Key"
				Layout.fillWidth: true
			}

			Button {
				id: addButton
				text: qsTr("Add")
				onClicked: {
					viewModel.addData(addField.text);
					addField.clear();
				}
			}

			Button {
				id: syncButton
				text: qsTr("Sync Status")
				onClicked: viewModel.showSyncInfo()
				Layout.columnSpan: 2
				Layout.fillWidth: true
			}
		}
	}
}

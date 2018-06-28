import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1
import de.skycoder42.QtMvvm.Sample 1.1

Page {
	id: sampleView
	property SampleViewModel viewModel: null
	readonly property bool presentAsRoot: true

	header: ContrastToolBar {
		RowLayout {
			anchors.fill: parent
			spacing: 0

			ActionButton {
				text: "≣"
				display: AbstractButton.TextOnly
				onClicked: QuickPresenter.toggleDrawer()
			}

			ToolBarLabel {
				text: qsTr("Sample")
				Layout.fillWidth: true
			}

			MenuButton {
				MenuItem {
					text: qsTr("Another Input")
					onTriggered: viewModel.getInput()
				}
				MenuItem {
					text: qsTr("Add Files")
					onTriggered: viewModel.getFiles()
				}
				MenuItem {
					text: qsTr("Add Color")
					onTriggered: viewModel.getColor()
				}
				MenuItem {
					text: qsTr("Show Progress")
					onTriggered: viewModel.showProgress()
				}

				MenuSeparator {}

				MenuItem {
					text: qsTr("About")
					onTriggered: viewModel.about()
				}
			}
		}
	}

	PresenterProgress {}

	Pane {
		anchors.fill: parent

		GridLayout {
			columns: 2
			anchors.fill: parent

			Label {
				text: qsTr("Name:")
			}

			TextField {
				id: nameEdit
				Layout.fillWidth: true
				selectByMouse: true

				MvvmBinding {
					viewModel: sampleView.viewModel
					view: nameEdit
					viewModelProperty: "name"
					viewProperty: "text"
				}
			}

			Label {
				text: qsTr("Active:")
			}

			Switch {
				id: activeSwitch
				Layout.alignment: Qt.AlignLeft

				MvvmBinding {
					viewModel: sampleView.viewModel
					view: activeSwitch
					viewModelProperty: "active"
					viewProperty: "checked"
				}
			}

			Label {
				text: qsTr("Events:")
			}

			RowLayout {
				Layout.fillWidth: true

				Button {
					text: qsTr("&Clear")
					onClicked: viewModel.clearEvents()
				}

				Button {
					text: qsTr("Get &Result")
					onClicked: viewModel.getResult()
				}
			}

			ListView {
				id: lView
				Layout.columnSpan: 2
				Layout.fillWidth: true
				Layout.fillHeight: true
				clip: true

				ScrollBar.vertical: ScrollBar {}

				model: viewModel.eventsModel

				delegate: ItemDelegate {
					width: parent.width
					text: viewModel.eventsModel.data(viewModel.eventsModel.index(index, 0))  //because "display" is not accessible
				}
			}
		}
	}
}

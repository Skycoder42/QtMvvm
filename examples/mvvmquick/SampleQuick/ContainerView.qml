import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1
import de.skycoder42.QtMvvm.Sample 1.1

Page {
	id: containerView
	property ContainerViewModel viewModel: null

	function presentItem(item) {
		return viewPlaceholder.presentItem(item);
	}

	function closeAction() {
		return viewPlaceholder.closeAction();
	}

	header: ContrastToolBar {
		ToolBarLabel {
			text: qsTr("View Container")
			anchors.fill: parent
		}
	}

	PresenterProgress {}

	Pane {
		anchors.fill: parent

		ColumnLayout {
			anchors.fill: parent

			ViewPlaceholder {
				id: viewPlaceholder

				viewModelType: containerView.viewModel.vmType
				parentViewModel: containerView.viewModel
				autoPresent: false

				Layout.fillWidth: true
				Layout.fillHeight: true

				BusyIndicator {
					anchors.centerIn: parent
					anchors.verticalCenterOffset: -(hookButton.height/2)
					running: !viewPlaceholder.loadedView
				}

				Button {
					id: hookButton
					Layout.fillWidth: true
					text: qsTr("Load from QML")
					onClicked: viewPlaceholder.presentView();
					enabled: !viewPlaceholder.loadedView

					anchors.left: parent.left
					anchors.right: parent.right
					anchors.bottom: parent.bottom
				}
			}

			Button {
				Layout.fillWidth: true
				text: viewPlaceholder.loadedView ? qsTr("Discard child") : qsTr("Show Child")
				onClicked: viewPlaceholder.loadedView ? viewPlaceholder.discardView() : viewModel.loadChild();
			}
		}
	}
}

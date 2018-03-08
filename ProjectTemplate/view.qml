import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Core 1.0
import de.skycoder42.QtMvvm.Quick 1.0
import com.example.%{ProjectLowerName} 1.0

Page {
	id: mainView
	property %{VmCn} viewModel: null

	header: ContrastToolBar {
		RowLayout {
			anchors.fill: parent
			spacing: 0

			ToolBarLabel {
				text: qsTr("%{VmClassName}")
				Layout.fillWidth: true
			}
@if '%{UseSettings}'

			MenuButton {
				MenuItem {
					id: settings
					text: qsTr("Settings")
					onClicked: viewModel.showSettings()
				}
			}
@endif
		}
	}

	PresenterProgress {}

	Pane {
		anchors.fill: parent

		ColumnLayout {
			anchors.fill: parent

			TextField {
				id: textEdit
				Layout.fillWidth: true

				MvvmBinding {
					viewModel: mainView.viewModel
					viewModelProperty: "text"
					view: textEdit
					viewProperty: "text"
				}
			}

			Label {
				id: textLabel
				Layout.fillWidth: true

				MvvmBinding {
					viewModel: mainView.viewModel
					viewModelProperty: "text"
					view: textLabel
					viewProperty: "text"
					type: MvvmBinding.OneWayToView
				}
			}

			Item {
				Layout.fillHeight: true
			}
		}
	}
}

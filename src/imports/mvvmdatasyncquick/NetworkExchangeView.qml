import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtDataSync 4.0
import de.skycoder42.QtMvvm.Core 1.0
import de.skycoder42.QtMvvm.Quick 1.0
import de.skycoder42.QtMvvm.DataSync.Core 1.0

Page {
	id: _networkExchangeView
	property NetworkExchangeViewModel viewModel: null

	header: ContrastToolBar {
		ToolBarLabel {
			id: _titleLabel
			anchors.fill: parent
			text: qsTr("Network Exchange")
		}
	}

	Pane {
		anchors.fill: parent
		ColumnLayout {
			id: _layout
			anchors.fill: parent

			Label {
				text: qsTr("Port:")
				Layout.fillWidth: true
				color: _portEdit.focus ? _nameEdit.selectionColor : palette.text
				opacity: _portEdit.focus ? 1 : 0.5
			}

			SpinBox {
				id: _portEdit
				Layout.fillWidth: true
				editable: true
				from: 0
				to: 65535

				MvvmBinding {
					viewModel: _networkExchangeView.viewModel
					viewModelProperty: "port"
					view: _portEdit
					viewProperty: "value"
				}
			}

			Label {
				text: qsTr("Name:")
				Layout.fillWidth: true
				color: _nameEdit.focus ? _nameEdit.selectionColor : palette.text
				opacity: _nameEdit.focus ? 1 : 0.5
			}

			TextField {
				id: _nameEdit
				Layout.fillWidth: true

				MvvmBinding {
					viewModel: _networkExchangeView.viewModel
					viewModelProperty: "deviceName"
					view: _nameEdit
					viewProperty: "text"
				}
			}

			Rectangle {
				Layout.fillWidth: true
				Layout.minimumHeight: 1
				Layout.maximumHeight: 1
				color: {
					if(QuickPresenter.currentStyle === "Material")
						return Material.foreground;
					else if(QuickPresenter.currentStyle === "Universal")
						return Universal.foreground;
					else
						return "black";
				}
			}

			Switch {
				id: _exchangeSwitch
				text: qsTr("Exchange active:")

				MvvmBinding {
					viewModel: _networkExchangeView.viewModel
					viewModelProperty: "active"
					view: _exchangeSwitch
					viewProperty: "checked"
				}
			}

			ListView {
				id: _exchangeList
				Layout.fillWidth: true
				Layout.fillHeight: true
				clip: true

				model: viewModel.sortedModel

				ScrollBar.vertical: ScrollBar {}

				delegate: ItemDelegate {
					width: parent.width

					contentItem: ColumnLayout {
						id: _delegateLayout
						spacing: 8

						Label {
							id: _nameLabel
							Layout.fillWidth: true
							text: name
						}

						Label {
							id: _addressLabel
							font.pointSize: _nameLabel.font.pointSize * 0.8
							Layout.fillWidth: true
							Layout.leftMargin: 8
							text: address
							opacity: 0.75
						}
					}

					onClicked: viewModel.exportTo(index)
				}
			}
		}
	}
}

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtDataSync 4.0
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1
import de.skycoder42.QtMvvm.DataSync.Core 1.1
import de.skycoder42.QtMvvm.DataSync.Quick 1.1

/*! @brief The view implementation for the QtMvvm::NetworkExchangeViewModel
 *
 * @extends QtQuick.Controls.Page
 *
 * @details This is the view used to present a network exchange view model. You can extend the
 * class if you need to extend that view.
 *
 * @sa QtMvvm::NetworkExchangeViewModel
 */
Page {
	id: _networkExchangeView

	/*! @brief The viewmodel to use
	 *
	 * @default{<i>Injected</i>}
	 *
	 * @accessors{
	 *	@memberAc{viewModel}
	 *  @notifyAc{viewModelChanged()}
	 * }
	 *
	 * @sa QtMvvm::NetworkExchangeViewModel
	 */
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

		ColorHelper {
			id: helper
		}

		ColumnLayout {
			id: _layout
			anchors.fill: parent

			DecorLabel {
				text: qsTr("Port:")
				Layout.fillWidth: true
				edit: _portEdit
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

			DecorLabel {
				text: qsTr("Name:")
				Layout.fillWidth: true
				edit: _nameEdit
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
				color: helper.text
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

			ScrollView {
				id: _exchangeScrollView

				Layout.fillWidth: true
				Layout.fillHeight: true
				clip: true

				ListView {
					id: _exchangeList

					model: viewModel.sortedModel
					delegate: ItemDelegate {
						width: _exchangeScrollView.width

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
}

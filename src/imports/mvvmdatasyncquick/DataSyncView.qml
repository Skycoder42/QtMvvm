import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtDataSync 4.0
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1
import de.skycoder42.QtMvvm.DataSync.Core 1.1

/*! @brief The view implementation for the QtMvvm::DataSyncViewModel
 *
 * @extends QtQuick.Controls.Page
 *
 * @details This is the view used to present a datasync view model. You can extend the class
 * if you need to extend that view.
 *
 * @sa QtMvvm::DataSyncViewModel
 */
Page {
	id: _dataSyncView

	/*! @brief The viewmodel to use
	 *
	 * @default{<i>Injected</i>}
	 *
	 * @accessors{
	 *	@memberAc{viewModel}
	 *  @notifyAc{viewModelChanged()}
	 * }
	 *
	 * @sa QtMvvm::DataSyncViewModel
	 */
	property DataSyncViewModel viewModel: null

	header: ContrastToolBar {
		id: _toolBar

		RowLayout {
			id: _toolLayout
			anchors.fill: parent
			spacing: 0

			ToolBarLabel {
				id: _titleLabel
				Layout.fillWidth: true
				text: qsTr("Synchronization")
			}

			ActionButton {
				id: _syncButton
				icon.name: "view-refresh"
				icon.source: "qrc:/de/skycoder42/qtmvvm/quick/icons/ic_sync.svg"
				text: qsTr("Synchronize")
				onClicked: viewModel.syncOrConnect()
			}

			ActionButton {
				id: _idButton
				icon.name: "fingerprint-gui"
				icon.source: "qrc:/de/skycoder42/qtmvvm/quick/icons/ic_fingerprint.svg"
				text: qsTr("Edit Identity")
				onClicked: viewModel.showDeviceInfo()
			}

			MenuButton {
				id: _moreButton

				MenuItem {
					text: qsTr("Update exchange key")
					onClicked: viewModel.accountManager.updateExchangeKey()
				}

				MenuSeparator {}

				MenuItem {
					text: qsTr("Reload devices list")
					onClicked: viewModel.accountManager.listDevices()
				}

				MenuSeparator {}

				MenuItem {
					text: qsTr("Change remote server")
					onClicked: viewModel.changeRemote()
				}

				MenuItem {
					text: qsTr("Reset Identity")
					onClicked: viewModel.performReset()
				}
			}
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
			spacing: 16

			Switch {
				id: _syncSwitch
				text: qsTr("Synchronization enabled")
				Layout.fillWidth: true

				MvvmBinding {
					viewModel: _dataSyncView.viewModel.syncManager
					viewModelProperty: "syncEnabled"
					view: _syncSwitch
					viewProperty: "checked"
				}
			}

			Label {
				id: _statusLabel
				Layout.fillWidth: true
				text: viewModel.statusString
				font.bold: true
				font.pointSize: 16
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
			}

			ProgressBar {
				id: _syncProgress
				Layout.fillWidth: true
				from: 0
				to: 1
				value: viewModel.syncManager.syncProgress
				visible: !_errorLabel.visible
			}

			Label {
				id: _errorLabel
				Layout.fillWidth: true
				wrapMode: Text.WordWrap
				text: viewModel.syncManager.lastError
				visible: text != ""
				color: "#aa0000"
				font.bold: true
			}

			Rectangle {
				Layout.fillWidth: true
				Layout.minimumHeight: 1
				Layout.maximumHeight: 1
				color: helper.text
			}

			Label {
				Layout.fillWidth: true
				text: qsTr("Other Devices:")
			}

			ScrollView {
				id: _devicesScrollView

				Layout.fillWidth: true
				Layout.fillHeight: true
				clip: true

				ListView {
					id: _devicesList

					model: viewModel.sortedModel
					delegate: SwipeDelegate {
						id: _swipeDelegate
						width: _devicesScrollView.width

						contentItem: ColumnLayout {
							id: _delegateLayout
							spacing: 8

							Label {
								id: _nameLabel
								Layout.fillWidth: true
								text: name
							}

							Label {
								id: _fpLabel
								font.pointSize: _nameLabel.font.pointSize * 0.8
								Layout.fillWidth: true
								Layout.leftMargin: 8
								text: fingerPrint
								elide: Text.ElideMiddle
								opacity: 0.75
							}
						}

						ListView.onRemove: SequentialAnimation {
							PropertyAction {
								target: _swipeDelegate
								property: "ListView.delayRemove"
								value: true
							}
							NumberAnimation {
								target: _swipeDelegate
								property: "height"
								to: 0
								easing.type: Easing.InOutQuad
							}
							PropertyAction {
								target: _swipeDelegate
								property: "ListView.delayRemove"
								value: false
							}
						}

						swipe.right: Rectangle {
							height: _devicesScrollView.height
							width: height
							anchors.right: parent.right
							color: {
								if(QuickPresenter.currentStyle === "Material")
									return Material.color(Material.Red);
								else if(QuickPresenter.currentStyle === "Universal")
									return Universal.color(Universal.Red);
								else
									return "#FF0000";
							}

							ActionButton {
								anchors.centerIn: parent
								implicitHeight: parent.height
								implicitWidth: parent.width

								icon.name: "user-trash"
								icon.source: "qrc:/de/skycoder42/qtmvvm/quick/icons/ic_delete_forever.svg"
								text: qsTr("Remove Device")

								Material.foreground: "white"
								Universal.foreground: "white"

								onClicked: {
									_swipeDelegate.swipe.close();
									viewModel.removeDevice(index)
								}
							}
						}
					}
				}
			}
		}
	}

	RoundMenuButton {
		id: _addButton

		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.margins: 16

		text: qsTr("Add new devices")
		icon.name: checked ? "tab-close" : "list-add"
		icon.source: checked ?
						 "qrc:/de/skycoder42/qtmvvm/quick/icons/ic_close.svg" :
						 "qrc:/de/skycoder42/qtmvvm/quick/icons/ic_add.svg"
		stickyToolTips: true

		Action {
			text: qsTr("Network Exchange")
			icon.name: "network-connect"
			icon.source: "qrc:/de/skycoder42/qtmvvm/quick/icons/ic_exchange.svg"

			onTriggered: viewModel.startNetworkExchange()
		}

		Action {
			text: qsTr("Export to file")
			icon.name: "document-export"
			icon.source: "qrc:/de/skycoder42/qtmvvm/quick/icons/ic_export.svg"

			onTriggered: viewModel.startExport()
		}

		Action {
			text: qsTr("Import from file")
			icon.name: "document-import"
			icon.source: "qrc:/de/skycoder42/qtmvvm/quick/icons/ic_import.svg"

			onTriggered: viewModel.startImport()
		}
	}
}

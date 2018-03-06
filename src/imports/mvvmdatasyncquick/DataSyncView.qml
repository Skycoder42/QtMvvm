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
	id: _dataSyncView
	property DataSyncViewModel viewModel: null

	header: ContrastToolBar {
		id: _toolBar

		RowLayout {
			id: _toolLayout
			anchors.fill: parent
			spacing: 0

			Label {
				id: _titleLabel
				font.pointSize: 14
				font.bold: true
				elide: Label.ElideRight
				horizontalAlignment: Qt.AlignLeft
				verticalAlignment: Qt.AlignVCenter
				Layout.fillWidth: true
				Layout.leftMargin: 16

				text: qsTr("Synchronization")
			}

			ActionButton {
				id: _syncButton
				icon.name: "view-refresh"
				icon.source: "image://svg/de/skycoder42/qtmvvm/quick/icons/ic_sync"
				text: qsTr("Synchronize")
				onClicked: viewModel.syncOrConnect()
			}

			ActionButton {
				id: _idButton
				icon.source: "image://svg/de/skycoder42/qtmvvm/quick/icons/ic_fingerprint"
				text: qsTr("Edit Identity")
				onClicked: viewModel.showDeviceInfo()
			}

			ActionButton {
				id: _moreButton
				icon.source: "image://svg/de/skycoder42/qtmvvm/quick/icons/ic_more_vert"
				text: qsTr("More…")
				checkable: true
				checked: _moreMenu.visible

				MouseArea { //used to catch mouse events to prevent flickering
					visible: _moreMenu.visible
					anchors.fill: parent
				}

				Menu {
					id: _moreMenu
					visible: _moreButton.checked

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

				Component.onCompleted: {
					if(QuickPresenter.currentStyle !== "Material")
						_moreMenu.y = Qt.binding(function(){return _moreButton.height});
				}
			}
		}
	}

	Pane {
		anchors.fill: parent
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
				color: {
					if(QuickPresenter.currentStyle === "Material")
						return Material.foreground;
					else if(QuickPresenter.currentStyle === "Universal")
						return Universal.foreground;
					else
						return "black";
				}
			}

			Label {
				Layout.fillWidth: true
				text: qsTr("Other Devices:")
			}

			ListView {
				id: _devicesList
				Layout.fillWidth: true
				Layout.fillHeight: true
				clip: true
				model: viewModel.accountModel

				ScrollBar.vertical: ScrollBar {}

				delegate: SwipeDelegate {
					id: _swipeDelegate
					width: parent.width

					contentItem: ColumnLayout {
						id: _delegateLayout
						spacing: 8

						Label {
							id: _nameLabel
							Layout.column: 0
							Layout.row: 0
							Layout.fillWidth: true
							text: name
						}

						Label {
							id: _fpLabel
							Layout.column: 0
							Layout.row: 1
							font.pointSize: _nameLabel.font.pointSize * 0.8
							Layout.fillWidth: true
							Layout.leftMargin: 7
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
						height: parent.height
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
							icon.source: "image://svg/de/skycoder42/qtmvvm/quick/icons/ic_delete_forever"
							text: qsTr("Remove Device")

							Material.foreground: "white"
							Universal.foreground: "white"

							onClicked: viewModel.accountModel.removeDevice(index)
						}
					}
				}
			}
		}
	}

	RoundActionButton {
		id: _addButton
		z: 7
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.margins: 16
		checkable: true
		text: qsTr("Add new devices")
		icon.name: checked ? "tab-close" : "list-add"
		icon.source: checked ?
						 "image://svg/de/skycoder42/qtmvvm/quick/icons/ic_close" :
						 "image://svg/de/skycoder42/qtmvvm/quick/icons/ic_add"
	}

	SubButton {
		id: _exchangeButton
		z: 3
		reference: _addButton
		expanded: _addButton.checked

		text: qsTr("Network Exchange")
		icon.name: "network-connect"
		icon.source: "image://svg/de/skycoder42/qtmvvm/quick/icons/ic_exchange"

		onClicked: {
			viewModel.startNetworkExchange();
			_addButton.checked = false;
		}
	}

	SubButton {
		id: _exportButton
		z: 3
		reference: _exchangeButton
		expanded: _addButton.checked

		text: qsTr("Export to file")
		icon.name: "document-export"
		icon.source: "image://svg/de/skycoder42/qtmvvm/quick/icons/ic_export"

		onClicked: {
			viewModel.startExport();
			_addButton.checked = false;
		}
	}

	SubButton {
		id: _importButton
		z: 3
		reference: _exportButton
		expanded: _addButton.checked

		text: qsTr("Import from file")
		icon.name: "document-import"
		icon.source: "image://svg/de/skycoder42/qtmvvm/quick/icons/ic_import"

		onClicked: {
			viewModel.startImport();
			_addButton.checked = false;
		}
	}
}

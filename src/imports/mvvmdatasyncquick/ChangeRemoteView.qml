import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtDataSync 4.0
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1
import de.skycoder42.QtMvvm.DataSync.Core 1.1
import de.skycoder42.QtMvvm.Quick.Private 1.0

Page {
	id: _changeRemoteView
	property PChangeRemoteViewModel viewModel: null

	header: ContrastToolBar {
		id: _toolBar

		RowLayout {
			id: _toolLayout
			anchors.fill: parent
			spacing: 0

			ActionButton {
				id: _cancelButton
				icon.name: "gtk-cancel"
				icon.source: "qrc:/de/skycoder42/qtmvvm/quick/icons/ic_close.svg"
				text: qsTr("Cancel")
				onClicked: QuickPresenter.popView()
			}

			ToolBarLabel {
				id: _titleLabel
				Layout.fillWidth: true
				text: qsTr("Change Remote")
			}

			ActionButton {
				id: _syncButton
				text: qsTr("Change")
				enabled: viewModel.valid
				icon.name: "gtk-apply"
				icon.source: "qrc:/de/skycoder42/qtmvvm/quick/icons/ic_check.svg"
				display: AbstractButton.TextBesideIcon
				rightPadding: 16

				onClicked: {
					if(viewModel.completeSetup())
						QuickPresenter.popView()
				}
			}
		}
	}

	Pane {
		anchors.fill: parent
		ColumnLayout {
			id: _layout
			anchors.fill: parent

			DecorLabel {
				text: qsTr("Remote url:")
				Layout.fillWidth: true
				edit: _urlEdit
			}

			TextField {
				id: _urlEdit
				Layout.fillWidth: true

				placeholderText: "wss://example.org/qdsapp/"
				validator: UrlValidator {
					allowedSchemes: ["ws", "wss"]
				}

				MvvmBinding {
					viewModel: _changeRemoteView.viewModel
					viewModelProperty: "url"
					view: _urlEdit
					viewProperty: "text"
					type: MvvmBinding.OneWayToViewModel
				}
			}

			DecorLabel {
				text: qsTr("Access key:")
				Layout.fillWidth: true
				edit: _accessKeyEdit
			}

			TextField {
				id: _accessKeyEdit
				Layout.fillWidth: true
				echoMode: TextInput.Password
				placeholderText: "Optional access secret"

				MvvmBinding {
					viewModel: _changeRemoteView.viewModel
					viewModelProperty: "accessKey"
					view: _accessKeyEdit
					viewProperty: "text"
					type: MvvmBinding.OneWayToViewModel
				}
			}

			DecorLabel {
				text: qsTr("Keep-Alive timout:")
				Layout.fillWidth: true
				edit: _keepAliveEdit
			}

			SpinBox {
				id: _keepAliveEdit
				Layout.fillWidth: true
				editable: true
				from: 1
				to: 1440

				MvvmBinding {
					viewModel: _changeRemoteView.viewModel
					viewModelProperty: "accessKey"
					view: _keepAliveEdit
					viewProperty: "value"
				}
			}

			CheckBox {
				id: _keepDataBox
				text: qsTr("Keep data")

				MvvmBinding {
					viewModel: _changeRemoteView.viewModel
					viewModelProperty: "keepData"
					view: _keepDataBox
					viewProperty: "checked"
				}
			}

			DecorLabel {
				text: qsTr("Extra Headers:")
				Layout.fillWidth: true
				edit: _headerScrollView
			}

			ScrollView {
				id: _headerScrollView
				Layout.fillWidth: true
				Layout.fillHeight: true
				clip: true

				ListView {
					id: _headerList

					model: viewModel.headerModel

					delegate: ItemDelegate {
						width: _headerScrollView.width
						text: qsTr("%1: %2").arg(key).arg(value)

						Button {
							id: _rmButton
							flat: true
							icon.width: 24
							icon.height: 24
							icon.name: "user-trash"
							icon.source: "qrc:/de/skycoder42/qtmvvm/quick/icons/ic_delete_forever.svg"
							anchors.right: parent.right
							implicitHeight: parent.height
							implicitWidth: implicitHeight

							onClicked: viewModel.removeHeaderConfig(index)
						}
					}

					footer: RowLayout {
						width: _headerScrollView.width

						TextField {
							id: _keyEdit
							placeholderText: qsTr("Key")
							Layout.fillWidth: true
						}

						TextField {
							id: _valueEdit
							placeholderText: qsTr("Value")
							Layout.fillWidth: true
						}

						Button {
							id: _addButton
							flat: true
							enabled: _keyEdit.text !== ""
							icon.width: 24
							icon.height: 24
							icon.name: "list-add"
							icon.source: "qrc:/de/skycoder42/qtmvvm/quick/icons/ic_add.svg"

							onClicked: {
								viewModel.addHeaderConfig(_keyEdit.text, _valueEdit.text);
								_keyEdit.clear();
								_valueEdit.clear();
							}
						}
					}
				}
			}
		}
	}
}

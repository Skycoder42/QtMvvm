import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Core 1.0
import de.skycoder42.QtMvvm.Quick 1.0

Page {
	id: _settingsView
	property SettingsViewModel viewModel: null

	function closeAction() {
		return _settingsStack.closeAction();
	}

	header: ToolBar {
		id: _toolBar

		RowLayout {
			id: _toolLayout
			anchors.fill: parent
			spacing: 0

			ActionButton {
				id: _backButton
				source: "image://svg/de/skycoder42/qtmvvm/quick/icons/ic_arrow_back"
				toolTip: qsTr("Go back")
				onClicked: {
					//TODO close settings view
					//TODO remove, not needed
				}
			}

			Item {
				id: _labelContainer

				Layout.fillWidth: true
				Layout.minimumHeight: 56

				Label {
					id: _titleLabel
					font.pointSize: 16
					font.bold: true
					elide: Label.ElideRight
					leftPadding: 10
					horizontalAlignment: Qt.AlignLeft
					verticalAlignment: Qt.AlignVCenter
					anchors.fill: parent

					text: qsTr("Settings")
					visible: !_searchField.visible
				}

				TextField {
					id: _searchField
					horizontalAlignment: Qt.AlignLeft
					verticalAlignment: Qt.AlignVCenter
					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					height: Math.min(implicitHeight, parent.height)
					width: parent.width
				}
			}

			ActionButton {
				id: _searchButton
				visible: true
				toolTip: qsTr("Search in settings")
				onClicked: toggleSearchState()
			}

			ActionButton {
				id: _restoreButton
				visible: true
				source: "image://svg/de/skycoder42/qtmvvm/quick/icons/ic_settings_backup_restore"
				toolTip: qsTr("Restore settings")
				onClicked: builder.restoreDefaults()
			}
		}
	}

	states: [
		State {
			name: "title"
			PropertyChanges {
				target: _searchButton
				source: "image://svg/de/skycoder42/qtmvvm/quick/icons/ic_search"
			}
			PropertyChanges {
				target: _titleLabel
				visible: true
			}
			PropertyChanges {
				target: _searchField
				visible: false
				width: 0
			}
			StateChangeScript {
				name: "focusScript"
				script: _searchField.clear();
			}
		},
		State {
			name: "search"
			PropertyChanges {
				target: _searchButton
				source: "image://svg/de/skycoder42/qtmvvm/quick/icons/ic_close"
			}
			PropertyChanges {
				target: _titleLabel
				visible: false
			}
			PropertyChanges {
				target: _searchField
				visible: true
				width: _labelContainer.width
			}
			StateChangeScript {
				name: "focusScript"
				script: _searchField.forceActiveFocus();
			}
		}
	]
	transitions: [
		Transition {
			from: "title"
			to: "search"
			SequentialAnimation {
				PropertyAnimation {
					target: _searchField
					property: "visible"
					duration: 0
				}
				PropertyAnimation {
					target: _searchField
					property: "width"
					duration: 250
					easing.type: Easing.InOutCubic
				}
				PropertyAnimation {
					target: _titleLabel
					property: "visible"
					duration: 0
				}
			}
		},
		Transition {
			from: "search"
			to: "title"
			SequentialAnimation {
				PropertyAnimation {
					target: _titleLabel
					property: "visible"
					duration: 0
				}
				PropertyAnimation {
					target: _searchField
					property: "width"
					duration: 250
					easing.type: Easing.InOutCubic
				}
				PropertyAnimation {
					target: _searchField
					property: "visible"
					duration: 0
				}
			}
		}
	]

	state: "title"

	function toggleSearchState() {
		if(state == "title")
			state = "search";
		else
			state = "title";
	}

	PresenterProgress {}

	StackView {
		id: _settingsStack
		anchors.fill: parent

		function closeAction() {
			if(_settingsStack.depth <= 1)
				return false;
			else {
				_settingsStack.pop();
				return true;
			}
		}
	}

//	SettingsUiBuilder {
//		id: builder
//		buildView: _settingsView
//		control: _settingsView.control
//		filterText: _searchField.text

//		onInitActions: {
//			searchButton.visible = allowSearch;
//			restoreButton.visible = allowSearch;
//		}

//		onCreateView: {
//			if(isOverview) {
//				_settingsStack.push("qrc:/de/skycoder42/qtmvvm/settings/quick/OverviewListView.qml", {
//									   "model": model,
//									   "showSections": showSections
//								   });
//			} else {
//				_settingsStack.push("qrc:/de/skycoder42/qtmvvm/settings/quick/SectionListView.qml", {
//									   "model": model
//								   });
//			}
//		}
//	}
}

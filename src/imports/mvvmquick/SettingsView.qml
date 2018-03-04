import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Core 1.0
import de.skycoder42.QtMvvm.Quick 1.0

Page {
	id: _settingsView
	property SettingsViewModel viewModel: null
	property bool fullClose: false

	function closeAction() {
		return !fullClose && _settingsStack.closeAction();
	}

	header: ContrastToolBar {
		id: _toolBar

		RowLayout {
			id: _toolLayout
			anchors.fill: parent
			spacing: 0

			Item {
				id: _labelContainer

				Layout.fillWidth: true
				Layout.fillHeight: true
				Layout.leftMargin: 16

				Label {
					id: _titleLabel
					font.pointSize: 14
					font.bold: true
					elide: Label.ElideRight
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
				visible: _builder.allowSearch
				text: qsTr("Search in settings")
				onClicked: toggleSearchState()
			}

			ActionButton {
				id: _restoreButton
				visible: _builder.allowRestore
				icon.source: "image://svg/de/skycoder42/qtmvvm/quick/icons/ic_settings_backup_restore"
				text: qsTr("Restore settings")
				onClicked: _builder.restoreDefaults()
			}
		}
	}

	states: [
		State {
			name: "title"
			PropertyChanges {
				target: _searchButton
				icon.name: "search"
				icon.source: "image://svg/de/skycoder42/qtmvvm/quick/icons/ic_search"
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
				icon.name: "gtk-close"
				icon.source: "image://svg/de/skycoder42/qtmvvm/quick/icons/ic_close"
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

	Component {
		id: _overviewComponent

		OverviewListView {
			id: __ovListView
			builder: _builder

			Component.onCompleted: _settingsStack.push(__ovListView)
		}
	}

	Component {
		id: _sectionViewComponent

		SectionListView {
			id: __secListView
			builder: _builder

			Component.onCompleted: _settingsStack.push(__secListView)
		}
	}

	SettingsUiBuilder {
		id: _builder
		buildView: _settingsView
		viewModel: _settingsView.viewModel
		filterText: _searchField.text

		onPresentOverview: _overviewComponent.incubateObject(_settingsStack, {
																	model: model,
																	showSections: hasSections
																}, Qt.Asynchronous)
		onPresentSection: _sectionViewComponent.incubateObject(_settingsStack, {
																   model: model
															   }, Qt.Asynchronous)

		onCloseSettings: {
			_settingsView.fullClose = true;
			QuickPresenter.popView();
		}
	}
}

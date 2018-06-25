import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Core 1.0
import de.skycoder42.QtMvvm.Quick 1.0

/*! @brief The view implementation for the QtMvvm::SettingsViewModel
 *
 * @extends QtQuick.Controls.Page
 *
 * @details This is the view used to present a settings view model. You can extend the class
 * if you need to extend that view.
 *
 * @sa QtMvvm::SettingsViewModel
 */
Page {
	id: _settingsView

	/*! @brief The viewmodel to use
	 *
	 * @default{<i>Injected</i>}
	 *
	 * @accessors{
	 *	@memberAc{viewModel}
	 *  @notifyAc{viewModelChanged()}
	 * }
	 *
	 * @sa QtMvvm::SettingsViewModel
	 */
	property SettingsViewModel viewModel: null
	/*! @brief Specifiy if a back action should always close the full settings
	 *
	 * @default{`false`}
	 *
	 * The settings view consists of an internal stack view to present settings pages. By
	 * default only one page is closed at a time. By setting this property to true, a close
	 * action will always close all of them.
	 *
	 * @accessors{
	 *	@memberAc{fullClose}
	 *  @notifyAc{fullCloseChanged()}
	 * }
	 */
	property bool fullClose: false

	/*! @brief Can be called to try to close a single settings page
	 *
	 * @return type:bool `true` if a page was closed, `false` if not
	 *
	 * This method is called by the presenter to close the pages of the settings view one at
	 * a time.
	 *
	 * @note if SettingsView::fullClose is true, this method will always return false.
	 *
	 * @sa SettingsView::fullClose, PresentingStackView::closeAction
	 */
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

				ToolBarLabel {
					id: _titleLabel
					anchors.fill: parent
					leftPadding: 0
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
				icon.source: "qrc:/de/skycoder42/qtmvvm/quick/icons/ic_settings_backup_restore.svg"
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
				icon.source: "qrc:/de/skycoder42/qtmvvm/quick/icons/ic_search.svg"
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
				icon.source: "qrc:/de/skycoder42/qtmvvm/quick/icons/ic_close.svg"
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

	//! @brief Can be called to toggle the state of the search bar
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

		ScrollView {
			id: __ovScrollView
			property alias model: __ovListView.model
			property alias showSections: __ovListView.showSections
			clip: true

			OverviewListView {
				id: __ovListView
				builder: _builder
			}

			Component.onCompleted: _settingsStack.push(__ovScrollView)
		}
	}

	Component {
		id: _sectionViewComponent

		ScrollView {
			id: __secScrollView
			property alias model: __secListView.model
			clip: true

			SectionListView {
				id: __secListView
				builder: _builder
			}

			Component.onCompleted: _settingsStack.push(__secScrollView)
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
															 }, Qt.Synchronous)
		onPresentSection: _sectionViewComponent.incubateObject(_settingsStack, {
																   model: model
															   }, Qt.Synchronous)
		onCloseSettings: {
			_settingsView.fullClose = true;
			QuickPresenter.popView();
		}
	}
}

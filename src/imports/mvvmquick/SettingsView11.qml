import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1

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

			SearchBar {
				id: _searchBar

				Layout.fillWidth: true
				Layout.fillHeight: true

				title: qsTr("Settings")
				allowSearch: _builder.allowSearch
				searchToolTip: qsTr("Search in settings")
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
		filterText: _searchBar.searchText

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

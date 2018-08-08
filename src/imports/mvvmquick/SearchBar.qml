import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Quick 1.1

/*! @brief A ToolBarLabel with a search button next to it, that can turn into a searchbar
 *
 * @extends QtQuick.Item
 *
 * This provides the standard "searchbar in titlebar" style control. In the normal state, it
 * displays a normal ToolBarLabel with the title in it, and a simple ActionButton as search
 * button next to it. When the button gets pressed, the label gets replaced with a TextField
 * and the user can enter a searchText. Clicking the button again closes the edit and clears
 * the text.
 *
 * @sa ToolBarLabel, ActionButton
 */
Item {
	id: _searchBar

	/*! @brief type:string The title to be shown in the ToolBarLabel when not searching
	 *
	 * @default{<i>empty</i>}
	 *
	 * @accessors{
	 *	@memberAc{title}
	 *  @notifyAc{titleChanged()}
	 * }
	 */
	property alias title: _titleLabel.text
	/*! @brief type:bool Specify, whether searching is currently allowed
	 *
	 * @default{`true`}
	 *
	 * If disallowed, the search button gets hidden and the title shown. The will not be able
	 * to start a search until this property gets set to true again.
	 *
	 * @accessors{
	 *	@memberAc{allowSearch}
	 *  @notifyAc{allowSearchChanged()}
	 * }
	 */
	property alias allowSearch: _searchButton.visible
	/*! @brief type:string The tooltip/text of the search button
	 *
	 * @default{`Search…` (translated)}
	 *
	 * @accessors{
	 *	@memberAc{searchToolTip}
	 *  @notifyAc{searchToolTipChanged()}
	 * }
	 */
	property alias searchToolTip: _searchButton.text
	/*! @brief Specifies, whether the bar is currently in search mode or in title mode
	 *
	 * @default{`false`}
	 *
	 * @accessors{
	 *	@memberAc{inSearchMode}
	 *  @notifyAc{inSearchModeChanged()}
	 *	@readonlyAc
	 * }
	 */
	readonly property bool inSearchMode: state == "search"

	/*! @brief type:string The current text of the search TextField
	 *
	 * @default{<i>empty</i>}
	 *
	 * You can use this property to get the text that was entered by the user and you can
	 * modify it yourself to change the entered search string.
	 *
	 * @accessors{
	 *	@memberAc{searchText}
	 *  @notifyAc{searchTextChanged()}
	 * }
	 */
	property alias searchText: _searchField.text
	/*! @brief type:ActionButton Specifies, whether the bar is currently in search mode or in title mode
	 *
	 * @default{<i>reference to the internal button</i>}
	 *
	 * You can use this reference to access the internally used button
	 *
	 * @accessors{
	 *	@memberAc{searchButton}
	 *	@readonlyAc
	 * }
	 */
	readonly property alias searchButton: _searchButton

	//! @brief Can be called to ensure the title is shown and not the search bar
	function showTitle() {
		state = "title";
	}

	/*!
	 * @brief Can be called to open the search bar
	 *
	 * @param type:string text An optional text to set a search text
	 *
	 * Opens the searchbar, if not already open, and replaces the search text by text, if
	 * valid, or does not change it.
	 */
	function showSearchBar(text) {
		if(typeof text === "string")
			searchText = text
		state = "search";
	}

	//! @brief Can be called to toggle the state of the search bar
	function toggleSearchState() {
		if(!allowSearch)
			return;
		if(inSearchMode)
			showTitle();
		else
			showSearchBar();
	}

	onAllowSearchChanged: {
		if(!allowSearch)
			showTitle();
	}

	RowLayout {
		id: _barLayout
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
			text: qsTr("Search…")
			onClicked: toggleSearchState()
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
				name: "clearScript"
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
}

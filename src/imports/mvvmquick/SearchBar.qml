import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Quick 1.1

Item {
	id: _searchBar

	property alias title: _titleLabel.text
	property alias allowSearch: _searchButton.visible
	property alias searchToolTip: _searchButton.text
	property bool inSearchMode: state == "search"

	property alias searchText: _searchField.text

	function showTitle() {
		state = "title";
	}

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

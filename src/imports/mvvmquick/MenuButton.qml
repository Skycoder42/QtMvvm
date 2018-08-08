import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.QtMvvm.Quick 1.1

/*! @brief An extension of the ActionButton to provide a simple "more menu" button
 *
 * @details The button already has a text, icon and tooltip and it will automatically display
 * a context menu when pressed. Add the menu items as the children of the button:
 *
 * @code{.qml}
 * MenuButton {
 *		id: mButton
 *
 *		MenuItem {
 *			text: "Copy"
 *		}
 *
 *		MenuItem {
 *			text: "Paste"
 *		}
 *
 *		//...
 * }
 * @endcode
 *
 * @sa RoundActionButton
 */
ActionButton {
	id: _menuButton
	icon.name: "view-more-symbolic"
	icon.source: "qrc:/de/skycoder42/qtmvvm/quick/icons/ic_more_vert.svg"
	text: qsTr("More…")
	checkable: true
	checked: _moreMenu.visible

	/*! @brief type:Menu A reference to the menu this button is showing
	 *
	 * @default{<i>A @ref QtQuick.Controls.Menu "Menu" instance</i>}
	 *
	 * @accessors{
	 *	@memberAc{moreMenu}
	 *	@readonlyAc
	 * }
	 *
	 * @sa menuContent
	 */
	readonly property alias moreMenu: _moreMenu
	/*! @brief type:list The items that should be displayed in the menu
	 *
	 * See the @ref QtQuick.Controls.Menu "Menu.contentData" documentation for a
	 * description of this property.
	 *
	 * @accessors{
	 *	@memberAc{menuContent}
	 *  @notifyAc{menuContentChanged()}
	 *	@defaultAc
	 * }
	 *
	 * @sa moreMenu
	 */
	default property alias menuContent: _moreMenu.contentData

	MouseArea { //used to catch mouse events to prevent flickering
		visible: _moreMenu.visible
		anchors.fill: parent
	}

	Menu {
		id: _moreMenu
		visible: _menuButton.checked
	}

	Component.onCompleted: {
		if(QuickPresenter.currentStyle !== "Material") {
			_moreMenu.y = Qt.binding(function(){
				return _menuButton.height + Math.max(0, (parent.height - _menuButton.height)/2);
			});
		} else {
			_moreMenu.y = Qt.binding(function(){
				return Math.min(0, (_menuButton.height - parent.height)/2);
			});
		}
	}
}

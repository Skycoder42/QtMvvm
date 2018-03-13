import QtQuick 2.10
import QtQuick.Controls 2.3

/*! @brief A @ref QtQuick.Controls.Drawer "Drawer" that can be used as a presenter for drawer
 * views
 *
 * @extends QtQuick.Controls.Drawer
 *
 * @details You can use this class as presenter for drawer views. Those are views that are not
 * drawers themselves, but need to be placed in one in order to properly work. Use it as
 * follows:
 *
 * @code{.qml}
 * MyPresenter {
 *		id: _root
 *
 *		PresentingDrawer {
 *			id: _rootDrawer
 *		}
 *
 *		function presentDrawerContent(item) {
 *			return _rootDrawer.presentDrawerContent(item);
 *		}
 *
 *		function toggleDrawer() {
 *			_rootDrawer.toggle();
 *		}
 *
 *		function closeAction() {
 *			var closed = false;
 *			// ...
 *			if(!closed)
 *				closed = _rootDrawer.closeAction();
 *			// ...
 *			return closed;
 *		}
 * }
 * @endcode
 *
 * @sa QtMvvmApp
 */
Drawer {
	id: _presentingDrawer

	width: Math.min(300, parent.width - 24);
	height: parent.height - y

	//! Internal property
	property Item _mainChild: null

	//! Toggles the current drawer state. It closes an open drawer or opens a closed one
	function toggle() {
		if(visible)
			close();
		else
			open();
	}

	/*! @brief The primary presenting method to present a drawer item
	 *
	 * @param type:Item item The item to be shown
	 * @return type:bool `true` if successfully presented, `false` if not
	 *
	 * Replaces the current drawer content by the new item. The old content is deleted and the
	 * new item set.
	 *
	 * @sa QtMvvmApp::presentDrawerContent, @ref QtQuick.Item "Item"
	 */
	function presentDrawerContent(item) {
		if(_mainChild)
			_mainChild.destroy();
		item.parent = _presentingDrawer.contentItem;
		item.drawer = _presentingDrawer;
		_mainChild = item;
		return true;
	}

	/*! @brief Can be called to try to close open drawers
	 *
	 * @return type:bool `true` if a drawer was closed, `false` if not
	 *
	 * Use this method in your main presenter react on a close event. It will close the drawer
	 * in case it is open.
	 *
	 * @sa QtMvvmApp::closeAction
	 */
	function closeAction() {
		if(_mainChild && typeof _mainChild.closeAction == "function") {
			if(_mainChild.closeAction())
				return true;
		}

		if(visible) {
			close();
			return true;
		} else
			return false;
	}
}

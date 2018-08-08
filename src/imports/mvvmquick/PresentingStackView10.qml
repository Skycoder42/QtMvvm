import QtQuick 2.10
import QtQuick.Controls 2.3

/*! @brief A presentation helper that can present standard mvvm views
 *
 * @extends QtQuick.Controls.StackView
 *
 * @details You can use this class as presenter for normale views in case you create your own
 * root presenter instead of using the QtMvvmApp. This partial presenter can handle any view
 * that extends the @ref QtQuick.Item "Item" type. Use it as follows:
 *
 * @code{.qml}
 * MyPresenter {
 *		id: _root
 *
 *		PresentingStackView {
 *			id: _rootStack
 *			anchors.fill: parent
 *		}
 *
 *		function presentItem(item) {
 *			return _rootStack.presentItem(item);
 *		}
 *
 *		function closeAction() {
 *			var closed = false;
 *			// ...
 *			if(!closed)
 *				closed = _rootStack.closeAction();
 *			// ...
 *			return closed;
 *		}
 * }
 * @endcode
 *
 * @sa QtMvvmApp
 */
StackView {
	id: _presenterStack

	/*! @brief The duration of the default push and pop animations
	 *
	 * @default{`150` milliseconds}
	 *
	 * You can use this property to speed up or slow down the default animations
	 *
	 * @accessors{
	 *	@memberAc{animDuration}
	 *  @notifyAc{animDurationChanged()}
	 * }
	 *
	 * @sa opDuration
	 */
	property int animDuration: 150
	/*! @brief The duration of the fade out
	 *
	 * @default{`75` milliseconds}
	 *
	 * You can use this property to speed up or slow down the fading part of the push and
	 * pop animations.
	 *
	 * @accessors{
	 *	@memberAc{opDuration}
	 *  @notifyAc{opDurationChanged()}
	 * }
	 *
	 * @sa animDuration
	 */
	property int opDuration: 75

	//! Internal property
	property var _clearItems: []

	/*! @brief The primary presenting method to present a view
	 *
	 * @param type:Item item The item to be presented
	 * @return type:bool `true` if successfully presented, `false` if not
	 *
	 * Use this method in your main presenter to present items via this sub presenter.
	 *
	 * If the presented item has a boolean property named `presentAsRoot`, the item will
	 * replace all contents on the stack instead of beeing pushed on it.
	 *
	 * @sa QtMvvmApp::presentItem, @ref QtQuick.Item "Item"
	 */
	function presentItem(item) {
		if(typeof item.presentAsRoot == "boolean" && item.presentAsRoot) {
			if(safeReplace(null, item))
				return true;
			else
				return false;
		} else {
			if(push(item))
				return true;
			else
				return false;
		}
	}

	/*! @brief Can be called to try to close the presenters current top level view
	 *
	 * @return type:bool `true` if a view was closed, `false` if not
	 *
	 * Use this method in your main presenter react on a close event. It will pop the top
	 * level view from the stack, if one is present
	 *
	 * @sa QtMvvmApp::closeAction
	 */
	function closeAction() {
		if(_presenterStack.currentItem &&
		   typeof _presenterStack.currentItem.closeAction == "function") {
			if(_presenterStack.currentItem.closeAction())
				return true;
		}

		if(_presenterStack.depth <= 1)
			return false;
		else {
			if(_presenterStack.safePop())
				return true;
			else
				return false;
		}
	}

	/*! @brief Pop and delete a view
	 *
	 * @param type:Item item an optional item to be popped to
	 * @param type:int operation How to perform the operation
	 * @return type:bool `true` if a view was closed, `false` if not
	 *
	 * Use this method instead of the @ref QtQuick.Controls.StackView "pop()" method. It will
	 * not only pop the top level view, but also delete it. This is recommened to do for views
	 * presented.
	 *
	 * @sa PresentingStackView::closeAction, PresentingStackView::safeReplace
	 */
	function safePop(item, operation) {
		var resItem = pop(item, operation)
		if(resItem) {
			_clearItems.push(resItem);
			return true;
		} else
			return false;
	}

	/*! @brief replace and delete views
	 *
	 * @param type:Item target The new view to be placed on the stack
	 * @param type:Item item an optional item to be popped to
	 * @param type:object properties The presentation properties to be passed to the target
	 * @param type:int operation How to perform the operation
	 * @return type:bool `true` if the replacement was successful, `false` if not
	 *
	 * Use this method instead of the @ref QtQuick.Controls.StackView "replace()" method. It
	 * will not only replace the replaced views, but also delete them. This is recommened to
	 * do for views presented.
	 *
	 * @sa PresentingStackView::closeAction, PresentingStackView::safePop
	 */
	function safeReplace(target, item, properties, operation) {
		var items = [];
		for(var i = depth -1; i >= 0; i--) {
			var cItem = get(i, StackView.ForceLoad);
			_clearItems.push(cItem);
			if(cItem === target)
				break;
		}
		if(replace(target, item, properties, operation))
			return true;
		else
			return false;
	}

	//! Internal method
	function clearWaitingItems() {
		_clearItems.forEach(function(item) {
			if(typeof item.afterPop == "function")
				item.afterPop();
			item.destroy();
		});
		_clearItems = [];
	}

	pushEnter: Transition {
		PropertyAnimation {
			property: "y"
			easing.type: Easing.InOutQuad
			from: height * 0.3
			to: 0
			duration: _presenterStack.animDuration
		}
		PropertyAnimation {
			property: "opacity"
			from: 0.0
			to: 1.0
			duration: _presenterStack.opDuration
		}
	}
	pushExit: Transition {
		PauseAnimation {
			duration: _presenterStack.animDuration
		}

		onRunningChanged: {
			if(!running)
				Qt.callLater(clearWaitingItems)
		}
	}
	popEnter: Transition {
		PauseAnimation {
			duration: _presenterStack.animDuration
		}
	}
	popExit: Transition {
		PropertyAnimation {
			property: "y"
			easing.type: Easing.InOutQuad
			from: 0
			to: height * 0.3
			duration: _presenterStack.animDuration
		}
		SequentialAnimation {
			PauseAnimation {
				duration: _presenterStack.animDuration - _presenterStack.opDuration
			}
			PropertyAnimation {
				id: pp1
				property: "opacity"
				from: 1.0
				to: 0.0
				duration: _presenterStack.opDuration
			}
		}

		onRunningChanged: {
			if(!running)
				Qt.callLater(clearWaitingItems)
		}
	}
	replaceEnter: pushEnter
	replaceExit: pushExit
}

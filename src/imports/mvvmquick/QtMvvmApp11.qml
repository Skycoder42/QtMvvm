import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.QtMvvm.Quick 1.1

/*! @brief An application root window that is a full fledged QML presenter
 *
 * @extends QtQuick.Controls.ApplicationWindow
 *
 * @details This is the standard presenter you should use in your mvvm application as the root
 * QML component of the primary qml file loaded by the engine. It will automatically register
 * as the main QML presenter and can handle all the standard view types.
 *
 * @sa PresenterProgress, PresentingStackView, PresentingDrawer, PopupPresenter,
 * DialogPresenter, QuickPresenter
 */
ApplicationWindow {
	id: _root
	visible: true
	width: 360
	height: 520

	/*! @brief type:PresentingDrawer A reference to the PresentingDrawer
	 *
	 * @default{`null`}
	 *
	 * Returns the PresentingDrawer that is beeing used by the app. It is null by default as
	 * the presenter only gets created the first time a drawer view is beeing shown. From that
	 * point on, you can access the drawer via this property.
	 *
	 * @accessors{
	 *	@memberAc{drawer}
	 *  @notifyAc{drawerChanged()}
	 *	@readonlyAc
	 * }
	 *
	 * @sa PresentingDrawer, QtMvvmApp::presentDrawerContent, QtMvvmApp::rootOnlyDrawer
	 */
	readonly property alias drawer: _drawerLoader.item
	/*! @brief Specifies if the drawer can be accessed from the root element only
	 *
	 * @default{`true`}
	 *
	 * If set to `true`, the presenter can only be accessed by the user if only one view is
	 * currently presented on the internal PresentingStackView. As soon as second view gets
	 * pushed, the user cannot open the drawer anymore. By settings it to `false`, the drawer
	 * will always be accessible on any view on the stack.
	 *
	 * @accessors{
	 *	@memberAc{rootOnlyDrawer}
	 *  @notifyAc{rootOnlyDrawerChanged()}
	 * }
	 *
	 * @sa QtMvvmApp::presentDrawerContent, QtMvvmApp::drawer, QtMvvmApp::presentItem
	 */
	property bool rootOnlyDrawer: true

	PresenterProgress {
		id: _rootProgress
		z: _rootStack.empty ? 10 : -10
	}

	PresentingStackView {
		id: _rootStack
		anchors.fill: parent
	}

	Loader {
		id: _drawerLoader
		active: false
		asynchronous: false
		sourceComponent: PresentingDrawer {
			id: _rootDrawer

			interactive: !_root.rootOnlyDrawer || _rootStack.depth == 1
		}
	}

	PopupPresenter {
		id: _rootPopup
		rootItem: _root.contentItem
	}

	DialogPresenter {
		id: _rootDialogs
		rootItem: _root.contentItem
	}

	/*! @brief The presenting method to present a drawer item
	 *
	 * @param type:Item item The item to be shown
	 * @return type:bool `true` if successfully presented, `false` if not
	 *
	 * Calls PresentingDrawer::presentDrawerContent on the internally used presenting drawer.
	 * If no drawer has been presented yet, it is automatically created.
	 *
	 * @sa PresentingDrawer::presentDrawerContent, QtMvvmApp::drawer, @ref QtQuick.Item "Item"
	 */
	function presentDrawerContent(item) {
		if(!_drawerLoader.item)
			_drawerLoader.active = true;
		return _drawerLoader.item.presentDrawerContent(item);
	}

	/*! @brief The presenting method to present a view
	 *
	 * @param type:Item item The item to be presented
	 * @return type:bool `true` if successfully presented, `false` if not
	 *
	 * Calls PresentingStackView::presentItem on the internally used presenting stack view.
	 *
	 * @sa PresentingStackView::presentItem, @ref QtQuick.Item "Item"
	 */
	function presentItem(item) {
		return _rootStack.presentItem(item);
	}

	/*! @brief The presenting method to present a popup view
	 *
	 * @param type:Popup popup The popup to be presented
	 * @return type:bool `true` if successfully presented, `false` if not
	 *
	 * Calls PopupPresenter::presentPopup on the internally used popup presenter.
	 *
	 * @sa PopupPresenter::presentPopup, @ref QtQuick.Controls.Popup "Popup"
	 */
	function presentPopup(popup) {
		return _rootPopup.presentPopup(popup);
	}

	/*! @brief The presenting method to present a dialog
	 *
	 * @param type:MessageConfig config The message configuration to create a dialog of
	 * @param type:MessageResult result The result to report the dialog result to
	 * @return type:bool `true` if successfully presented, `false` if not
	 *
	 * Calls DialogPresenter::showDialog on the internally used dialog presenter.
	 *
	 * @sa QtMvvm::MessageConfig, QtMvvm::MessageResult, DialogPresenter::showDialog
	 */
	function showDialog(config, result) {
		return _rootDialogs.showDialog(config, result);
	}

	//! @copybrief PresentingDrawer::toggle
	function toggleDrawer() {
		if(_drawerLoader.item)
			_drawerLoader.item.toggle();
		else
			console.warn("No drawer like view active. Cannot toggle drawer");
	}

	/*! @brief Can be called to perform a close operation
	 *
	 * @return type:bool `true` if anything was closed, `false` if not
	 *
	 * Use this method to perform a close operation. The app will automatically check all
	 * internally used presenter in a logical order and call this method on all of them until
	 * one returns with a positive result. This way you have a global method to close any kind
	 * of active view. This is also used to handle window close events and the mobile back
	 * button.
	 *
	 * @sa PopupPresenter::closeAction, DialogPresenter::closeAction,
	 * PresentingDrawer::closeAction, PresentingStackView::closeAction
	 */
	function closeAction() {
		var closed = false;
		if(!closed)
			closed = _rootDialogs.closeAction();
		if(!closed)
			closed = _rootPopup.closeAction();
		if(!closed && _drawerLoader.item)
			closed = _drawerLoader.item.closeAction();
		if(!closed)
			closed = _rootStack.closeAction();
		//if everything was closed -> still accept it
		if(closed && _rootDialogs.emtpy && _rootPopup.empty && _rootStack.empty)
			closed = false;
		return closed;
	}

	Component.onCompleted: QuickPresenter.qmlPresenter = _root

	onClosing: close.accepted = !closeAction();
}

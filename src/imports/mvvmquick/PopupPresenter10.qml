import QtQuick 2.10
import QtQuick.Controls 2.3

/*! @brief A presentation helper that can present mvvm views that extend the
 * @ref QtQuick.Controls.Popup "Popup" type
 *
 * @extends QtQml.QtObject
 *
 * @details You can use this class as presenter for popup views in case you create your own
 * root presenter instead of using the QtMvvmApp. This partial presenter can handle any view
 * that extends the popup type. Use it as follows:
 *
 * @code{.qml}
 * MyPresenter {
 *		id: _root
 *
 *		PopupPresenter {
 *			id: _rootPopup
 *			rootItem: _root.contentItem
 *		}
 *
 *		function presentPopup(popup) {
 *			return _rootPopup.presentPopup(popup);
 *		}
 *
 *		function closeAction() {
 *			var closed = false;
 *			// ...
 *			if(!closed)
 *				closed = _rootPopup.closeAction();
 *			// ...
 *			return closed;
 *		}
 * }
 * @endcode
 *
 * @sa QtMvvmApp
 */
QtObject {
	id: _popPresenter

	/*! @brief The root item to present popup views to
	 *
	 * @default{`null`}
	 *
	 * This propety **must** be set in order for the presenter to work. It will create new
	 * popups with that item as parent, which decides in which relative are they are
	 * displayed. Unless you want to explicitly reposition popups, this should always be the
	 * root item of the root component of your application.
	 *
	 * @accessors{
	 *	@memberAc{rootItem}
	 *  @notifyAc{rootItemChanged()}
	 * }
	 */
	property Item rootItem: null
	
	//! Internal property
	property var _popups: []

	/*! @brief The primary presenting method to present a popup view
	 *
	 * @param type:Popup popup The popup to be presented
	 * @return type:bool `true` if successfully presented, `false` if not
	 *
	 * Use this method in your main presenter to present popups via this sub presenter.
	 *
	 * @sa QtMvvmApp::presentPopup, @ref QtQuick.Controls.Popup "Popup"
	 */
	function presentPopup(popup) {
		popup.parent = rootItem;
		popup.closed.connect(function() {
			var index = _popups.indexOf(popup);
			if(index > -1) {
				popup.destroy();
				_popups.splice(index, 1);
			}
		});
		_popups.push(popup);
		popup.open();
		return true;
	}

	/*! @brief Can be called to try to close open popups
	 *
	 * @return type:bool `true` if a popup was closed, `false` if not
	 *
	 * Use this method in your main presenter react on a close event. It will close the
	 * latest top level popup, if one is present.
	 *
	 * @sa QtMvvmApp::closeAction
	 */
	function closeAction() {
		if(_popups.length > 0) {
			_popups[_popups.length - 1].close();
			return true;
		} else
			return false;
	}
}

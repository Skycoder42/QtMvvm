import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1

/*! @brief A presentation helper that can present generic mvvm dialogs
 *
 * @extends QtQml.QtObject
 *
 * @details You can use this class as presenter for dialogs in case you create your own root
 * presenter instead of using the QtMvvmApp. This partial presenter can handle simple dialogs
 * (the ones shown via QtMvvm::CoreApp::showDialog). Use it as follows:
 *
 * @code{.qml}
 * MyPresenter {
 *		id: _root
 *
 *		DialogPresenter {
 *			id: _rootDialogs
 *			rootItem: _root.contentItem
 *		}
 *
 *		function showDialog(config, result) {
 *			return _rootDialogs.showDialog(config, result);
 *		}
 *
 *		function closeAction() {
 *			var closed = false;
 *			// ...
 *			if(!closed)
 *				closed = _rootDialogs.closeAction();
 *			// ...
 *			return closed;
 *		}
 * }
 * @endcode
 *
 * @sa QtMvvmApp
 */
QtObject {
	id: _dialogPresenter

	/*! @brief The root item to present dialogs to
	 *
	 * @default{`null`}
	 *
	 * This propety **must** be set in order for the presenter to work. It will create new
	 * dialogs with that item as parent, which decides in which relative are they are
	 * displayed. Unless you want to explicitly reposition dialogs, this should always be the
	 * root item of the root component of your application.
	 *
	 * @accessors{
	 *	@memberAc{rootItem}
	 *  @notifyAc{rootItemChanged()}
	 * }
	 */
	property Item rootItem: null

	/*! @brief Checks if the presenter has no open dialogs
	 *
	 * @default{`false`}
	 *
	 * As soon as there is at least a single open dialog, this property gets false. Only when
	 * no dialogs are show is it true. This property is always updated from within the
	 * closeAction() method, so you can be sure that it is true after the last dialog was
	 * closed that way.
	 *
	 * @accessors{
	 *	@memberAc{empty}
	 *  @notifyAc{emptyChanged()}
	 * }
	 */
	readonly property bool empty: _popups.length == 0

	/*! @brief The primary presenting method to present a dialog
	 *
	 * @param type:MessageConfig config The message configuration to create a dialog of
	 * @param type:MessageResult result The result to report the dialog result to
	 * @return type:bool `true` if successfully presented, `false` if not
	 *
	 * Use this method in your main presenter to present dialogs via this sub presenter.
	 *
	 * @sa QtMvvm::MessageConfig, QtMvvm::MessageResult, QtMvvmApp::showDialog
	 */
	function showDialog(config, result) {
		if(config.type == "msgbox")
			return createMsgBox(config, result)
		else if(config.type == "input")
			return createInput(config, result)
		else if(config.type == "file")
			return createFile(config, result)
		else if(config.type == "color")
			return createColor(config, result)
		else if(config.type == "progress")
			return createProgress(config, result)
		else
			return false;
	}

	/*! @brief Can be called to try to close open dialogs
	 *
	 * @return type:bool `true` if a dialog was closed, `false` if not
	 *
	 * Use this method in your main presenter react on a close event. It will close the
	 * latest top level dialog, if one is present.
	 *
	 * @sa QtMvvmApp::closeAction
	 */
	function closeAction() {
		if(_popups.length > 0) {
			if(typeof _popups[_popups.length - 1].closeAction == "function") {
				if(_popups[_popups.length - 1].closeAction())
					return true;
			}
			_popups[_popups.length - 1].reject();
			return true;
		} else
			return false;
	}

	//! Internal property
	property var _popups: []
	//! Internal property
	property Component _msgBoxComponent: MsgBox {
		id: __msgBox

		onClosed: {
			var index = _popups.indexOf(__msgBox);
			if(index > -1) {
				__msgBox.destroy();
				_dialogPresenter._popups.splice(index, 1);
			}
		}

		Component.onCompleted: {
			_popups.push(__msgBox)
			__msgBox.open()
		}
	}

	//! Internal property
	property Component _inputComponent: InputDialog {
		id: __input

		onClosed: {
			var index = _popups.indexOf(__input);
			if(index > -1) {
				__input.destroy();
				_dialogPresenter._popups.splice(index, 1);
			}
		}

		Component.onCompleted: {
			_popups.push(__input)
			__input.open()
		}
	}

	//! Internal property
	property Component _fileComponent: FileDialog {
		id: __file

		onClosed: {
			var index = _popups.indexOf(__file);
			if(index > -1) {
				__file.destroy();
				_dialogPresenter._popups.splice(index, 1);
			}
		}

		Component.onCompleted: {
			_popups.push(__file)
			__file.open()
		}
	}

	//! Internal property
	property Component _folderComponent: FolderDialog {
		id: __folder

		onClosed: {
			var index = _popups.indexOf(__folder);
			if(index > -1) {
				__folder.destroy();
				_dialogPresenter._popups.splice(index, 1);
			}
		}

		Component.onCompleted: {
			_popups.push(__folder)
			__folder.open()
		}
	}

	//! Internal property
	property Component _progressComponent: ProgressDialog {
		id: __progress

		onClosed: {
			var index = _popups.indexOf(__progress);
			if(index > -1) {
				__progress.destroy();
				_dialogPresenter._popups.splice(index, 1);
			}
		}

		Component.onCompleted: {
			_popups.push(__progress)
			__progress.open()
		}
	}

	/*! @brief Method present a dialog of the QtMvvm::MessageConfig::TypeMessageBox
	 *
	 * @param type:MessageConfig config The message configuration to create a dialog of
	 * @param type:MessageResult result The result to report the dialog result to
	 * @return type:bool `true` if successfully presented, `false` if not
	 *
	 * Used by the showDialog() method to show a dialog of the message box type. You can use
	 * it yourself if you want to extend the presenter and still keep the default dialogs it
	 * supports.
	 *
	 * @sa DialogPresenter::showDialog
	 */
	function createMsgBox(config, result) {
		var props = config.viewProperties;
		props["msgConfig"] = config;
		props["msgResult"] = result;
		var incubator = _msgBoxComponent.incubateObject(rootItem, props, Qt.Synchronous);
		return incubator.status !== Component.Error;
	}

	/*! @brief Method present a dialog of the QtMvvm::MessageConfig::TypeInputDialog
	 *
	 * @param type:MessageConfig config The message configuration to create a dialog of
	 * @param type:MessageResult result The result to report the dialog result to
	 * @return type:bool `true` if successfully presented, `false` if not
	 *
	 * Used by the showDialog() method to show a dialog of the input dialog type. You can use
	 * it yourself if you want to extend the presenter and still keep the default dialogs it
	 * supports.
	 *
	 * @sa DialogPresenter::showDialog
	 */
	function createInput(config, result) {
		var props = config.viewProperties;
		props["msgConfig"] = config;
		props["msgResult"] = result;
		var incubator = _inputComponent.incubateObject(rootItem, props, Qt.Synchronous);
		return incubator.status !== Component.Error;
	}

	/*! @brief Method present a dialog of the QtMvvm::MessageConfig::TypeFileDialog
	 *
	 * @param type:MessageConfig config The message configuration to create a dialog of
	 * @param type:MessageResult result The result to report the dialog result to
	 * @return type:bool `true` if successfully presented, `false` if not
	 *
	 * Used by the showDialog() method to show a dialog of the file dialog type. You can use
	 * it yourself if you want to extend the presenter and still keep the default dialogs it
	 * supports.
	 *
	 * @sa DialogPresenter::showDialog
	 */
	function createFile(config, result) {
		var props = config.viewProperties;
		props["msgConfig"] = config;
		props["msgResult"] = result;
		var incubator = null;
		if(config.subType == "folder")
			incubator = _folderComponent.incubateObject(rootItem, props, Qt.Synchronous);
		else
			incubator = _fileComponent.incubateObject(rootItem, props, Qt.Synchronous);
		return incubator.status !== Component.Error;
	}

	/*! @brief Method present a dialog of the QtMvvm::MessageConfig::TypeColorDialog
	 *
	 * @param type:MessageConfig config The message configuration to create a dialog of
	 * @param type:MessageResult result The result to report the dialog result to
	 * @return type:bool `true` if successfully presented, `false` if not
	 *
	 * Used by the showDialog() method to show a dialog of the color dialog type. You can use
	 * it yourself if you want to extend the presenter and still keep the default dialogs it
	 * supports.
	 *
	 * @sa DialogPresenter::showDialog
	 */
	function createColor(config, result) {
		config.viewProperties["alpha"] = (config.subType == "argb");
		config.type = "input";
		config.subType = "QColor";
		return createInput(config, result);
	}

	/*! @brief Method present a dialog of the QtMvvm::MessageConfig::TypeProgressDialog
	 *
	 * @param type:MessageConfig config The message configuration to create a dialog of
	 * @param type:MessageResult result The result to report the dialog result to
	 * @return type:bool `true` if successfully presented, `false` if not
	 *
	 * Used by the showDialog() method to show a dialog of the progress dialog type. You can
	 * use it yourself if you want to extend the presenter and still keep the default dialogs
	 * it supports.
	 *
	 * @sa DialogPresenter::showDialog
	 */
	function createProgress(config, result) {
		var props = config.viewProperties;
		props["msgConfig"] = config;
		props["msgResult"] = result;
		var incubator = _progressComponent.incubateObject(rootItem, props, Qt.Synchronous);
		return incubator.status !== Component.Error;
	}
}

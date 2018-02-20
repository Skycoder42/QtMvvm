import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.QtMvvm.Core 1.0

QtObject {
	id: _dialogPresenter

	property Item rootItem: null

	function showDialog(config, result) {
		if(config.type == "msgbox") {
			createMsgBox(config, result)
			return true;
		} else if(config.type == "input") {
			createInput(config, result)
			return true;
		} else
			return false;
	}

	function closeAction() {
		if(_popups.length > 0) {
			_popups[_popups.length - 1].reject();
			return true;
		} else
			return false;
	}

	property var _popups: []
	property Component _msgBoxComponent: Component {
		MsgBox {
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
	}

	property Component _inputComponent: Component {
		InputDialog {
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
	}

	function createMsgBox(config, result) {
		var props = config.viewProperties;
		props["msgConfig"] = config;
		props["msgResult"] = result;
		var incubator = _msgBoxComponent.incubateObject(rootItem, props);
		return incubator.status !== Component.Error;
	}

	function createInput(config, result) {
		var props = config.viewProperties;
		props["msgConfig"] = config;
		props["msgResult"] = result;
		var incubator = _inputComponent.incubateObject(rootItem, props);
		return incubator.status !== Component.Error;
	}
}

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
		} else
			return false;
	}

	property Component _msgBoxComponent: Component {
		MsgBox {
			id: __msgBox
			Component.onCompleted: __msgBox.open()
		}
	}

	function createMsgBox(config, result) {
		var props = config.viewProperties;
		props["msgConfig"] = config;
		props["msgResult"] = result;
		var incubator = _msgBoxComponent.incubateObject(rootItem, props);
		return incubator.status !== Component.Error;
	}
}

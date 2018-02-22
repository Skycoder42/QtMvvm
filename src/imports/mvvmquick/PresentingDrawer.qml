import QtQuick 2.10
import QtQuick.Controls 2.3

Drawer {
	id: _presentingDrawer

	width: Math.min(300, parent.width - 24);
	height: parent.height - y

	property Item _mainChild: null

	function toggle() {
		if(visible)
			close();
		else
			open();
	}

	function presentDrawerContent(item) {
		if(_mainChild)
			_mainChild.destroy();
		item.parent = _presentingDrawer.contentItem;
		item.drawer = _presentingDrawer;
		_mainChild = item;
		return true;
	}

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

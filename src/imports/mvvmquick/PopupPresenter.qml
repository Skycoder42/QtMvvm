import QtQuick 2.10
import QtQuick.Controls 2.3

QtObject {
	id: _popPresenter

	property Item rootItem: null

	property var _popups: []

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

	function closeAction() {
		if(_popups.length > 0) {
			_popups[_popups.length - 1].close();
			return true;
		} else
			return false;
	}
}

import QtQuick 2.10
import QtQuick.Controls 2.3

QtObject {
	id: _popPresenter

	property var popups: []

	function presentPopup(root, popup) {
		popup.parent = root;
		popup.closed.connect(function() {
			var index = popups.indexOf(popup);
			if(index > -1) {
				popup.destroy();
				popups.splice(index, 1);
			}
		});
		popup.open();
		popups.push(popup);
		return true;
	}

	function closeAction() {
		if(popups.length > 0) {
			popups[popups.length - 1].close();
			return true;
		} else
			return false;
	}
}

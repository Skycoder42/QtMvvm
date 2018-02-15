import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.qtmvvm.quick 1.0

ApplicationWindow {
	id: _root
	visible: true
	width: 360
	height: 520

	PresenterProgress {
		z: -10  //keep it low so its hidden after the first view was shown
	}

	function presentItem(item) {
		return true;
	}

	function presentPopup(item) {
		return true;
	}

	Component.onCompleted: QuickPresenter.qmlPresenter = _root
}

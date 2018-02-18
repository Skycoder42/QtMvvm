import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.QtMvvm.Quick 1.0

ApplicationWindow {
	id: _root
	visible: true
	width: 360
	height: 520

	PresenterProgress {
		z: -10  //keep it low so its hidden after the first view was shown
	}

	PresentingStackView {
		id: mainStack
		anchors.fill: parent
	}

	function presentItem(item) {
		return mainStack.presentItem(item);
	}

	function presentPopup(item) {
		return true;
	}

	Component.onCompleted: QuickPresenter.qmlPresenter = _root

	onClosing: {
		var closed = false;//messageBox.closeAction();

//		if(!closed) {
//			if(popups.length > 0) {
//				popups[popups.length - 1].close();
//				closed = true;
//			}
//		}

		if(!closed)
			closed = mainStack.closeAction();

		close.accepted = !closed;
	}
}

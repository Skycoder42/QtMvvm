import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.QtMvvm.Quick 1.0

ApplicationWindow {
	id: _root
	visible: true
	width: 360
	height: 520

	PresenterProgress {
		id: _rootProgress
		z: _rootStack.empty ? 10 : -10
	}

	PopupPresenter {
		id: _rootPopup
	}

	PresentingStackView {
		id: _rootStack
		anchors.fill: parent
	}

	function presentDrawerContent(item) {
		return false
	}

	function presentItem(item) {
		return _rootStack.presentItem(item);
	}

	function presentPopup(popup) {
		return _rootPopup.presentPopup(contentItem, popup);
	}

	Component.onCompleted: QuickPresenter.qmlPresenter = _root

	onClosing: {
		var closed = false;//messageBox.closeAction();

		if(!closed)
			closed = _rootPopup.closeAction();
		if(!closed)
			closed = _rootStack.closeAction();

		close.accepted = !closed;
	}
}

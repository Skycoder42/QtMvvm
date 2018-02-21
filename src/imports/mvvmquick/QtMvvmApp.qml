import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.QtMvvm.Quick 1.0

ApplicationWindow {
	id: _root
	visible: true
	width: 360
	height: 520

	readonly property alias drawer: _drawerLoader.item
	property bool rootOnlyDrawer: true

	PresenterProgress {
		id: _rootProgress
		z: _rootStack.empty ? 10 : -10
	}

	PresentingStackView {
		id: _rootStack
		anchors.fill: parent
	}

	Loader {
		id: _drawerLoader
		active: false
		asynchronous: false
		sourceComponent: PresentingDrawer {
			id: _rootDrawer

			interactive: !_root.rootOnlyDrawer || _rootStack.depth == 1
		}
	}

	PopupPresenter {
		id: _rootPopup
		rootItem: _root.contentItem
	}

	DialogPresenter {
		id: _rootDialogs
		rootItem: _root.contentItem
	}

	function presentDrawerContent(item) {
		if(!_drawerLoader.item)
			_drawerLoader.active = true;
		return _drawerLoader.item.presentDrawerContent(item);
	}

	function presentItem(item) {
		return _rootStack.presentItem(item);
	}

	function presentPopup(popup) {
		return _rootPopup.presentPopup(popup);
	}

	function showDialog(config, result) {
		return _rootDialogs.showDialog(config, result);
	}

	function toggleDrawer() {
		if(_drawerLoader.item)
			_drawerLoader.item.toggle();
		else
			console.warn("No drawer like view active. Cannot toggle drawer");
	}

	Component.onCompleted: QuickPresenter.qmlPresenter = _root

	onClosing: {
		var closed = false;

		if(!closed && _drawerLoader.item)
			closed = _drawerLoader.item.closeAction();
		if(!closed)
			closed = _rootDialogs.closeAction();
		if(!closed)
			closed = _rootPopup.closeAction();
		if(!closed)
			closed = _rootStack.closeAction();

		close.accepted = !closed;
	}
}

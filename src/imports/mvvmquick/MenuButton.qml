import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.QtMvvm.Quick 1.0

ActionButton {
	id: _menuButton
	icon.name: "view-more-symbolic"
	icon.source: "image://svg/de/skycoder42/qtmvvm/quick/icons/ic_more_vert"
	text: qsTr("More…")
	checkable: true
	checked: _moreMenu.visible

	property alias moreMenu: _moreMenu
	default property alias menuContent: _moreMenu.contentData

	MouseArea { //used to catch mouse events to prevent flickering
		visible: _moreMenu.visible
		anchors.fill: parent
	}

	Menu {
		id: _moreMenu
		visible: _menuButton.checked
	}

	Component.onCompleted: {
		if(QuickPresenter.currentStyle !== "Material") {
			_moreMenu.y = Qt.binding(function(){
				return _menuButton.height + Math.max(0, (parent.height - _menuButton.height)/2);
			});
		} else {
			_moreMenu.y = Qt.binding(function(){
				return Math.min(0, (_menuButton.height - parent.height)/2);
			});
		}
	}
}

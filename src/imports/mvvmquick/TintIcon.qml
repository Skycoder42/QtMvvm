import QtQuick 2.10
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import de.skycoder42.QtMvvm.Quick 1.1

Item {
	id: _tintIcon

	property alias icon: _imgBtn.icon
	property alias source: _imgBtn.icon.source

	implicitWidth: _imgBtn.icon.width
	implicitHeight: _imgBtn.icon.height

	Displace {
		source: _imgBtn
		z: 10
		anchors.fill: parent
	}

	ActionButton {
		id: _imgBtn
		visible: false
		z: -10
		padding: 0
		anchors.fill: parent

		background: Item {}
	}
}

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import de.skycoder42.QtMvvm.Quick 1.1

Item {
	id: _tintIcon

	property size iconSize: Qt.size(24, 24)
	property alias tintColor: _overlay.color
	property alias source: _image.source

	Image {
		id: _image
		anchors.centerIn: parent
		fillMode: Image.PreserveAspectFit
		horizontalAlignment: Image.AlignHCenter
		verticalAlignment: Image.AlignVCenter
		width: iconSize.width
		height: iconSize.height
		sourceSize: iconSize
		visible: false
	}

	ColorHelper {
		id: helper
	}

	ColorOverlay {
		id: _overlay
		anchors.fill: _image
		source: _image
		color: helper.text
	}
}

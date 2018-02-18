import QtQuick 2.10
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import QtGraphicalEffects 1.0
import de.skycoder42.QtMvvm.Quick 1.0

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

	ColorOverlay {
		id: _overlay
		anchors.fill: _image
		source: _image
		color: {
			if(QuickPresenter.currentStyle === "Material")
				return Material.foreground;
			else if(QuickPresenter.currentStyle === "Universal")
				return Universal.foreground;
			else
				return "black";
		}
	}
}

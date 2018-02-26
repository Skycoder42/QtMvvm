import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import de.skycoder42.QtMvvm.Quick 1.0

Label {
	property string title

	width: parent.width
	font.bold: true
	font.capitalization: Font.SmallCaps
	padding: 14
	bottomPadding: 4
	text: title + qsTr(":")

	background: Rectangle {
		anchors.fill: parent
		color: {
			if(QuickPresenter.currentStyle === "Material")
				return Material.background;
			else if(QuickPresenter.currentStyle === "Universal")
				return Universal.background;
			else
				return "white";
		}

		Rectangle {
			anchors.left: parent.left
			anchors.bottom: parent.bottom
			anchors.right: parent.right
			height: 2
			color: {
				if(QuickPresenter.currentStyle === "Material")
					return Material.accent;
				else if(QuickPresenter.currentStyle === "Universal")
					return Universal.accent;
				else
					return "black";
			}
		}
	}
}

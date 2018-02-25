import QtQuick 2.8
import QtQuick.Controls 2.1
import de.skycoder42.quickextras 2.0

Label {
	property string title

	CommonStyle {
		id: style
	}

	width: parent.width
	font.bold: true
	font.capitalization: Font.SmallCaps
	padding: 14
	bottomPadding: 4
	text: title + qsTr(":")

	background: Rectangle {
		anchors.fill: parent
		color: style.sBackground

		Rectangle {
			anchors.left: parent.left
			anchors.bottom: parent.bottom
			anchors.right: parent.right
			height: 2
			color: style.accent
		}
	}
}

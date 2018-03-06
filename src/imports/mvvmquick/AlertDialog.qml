import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Window 2.2

Dialog {
	id: _alertDialog

	property real extraHeight: 0
	property real baseWidth: 300

	x: parent ? (parent.width - width) / 2 : 0
	y: parent ? deltaY() : 0
	width: parent ? Math.min(Math.max(implicitWidth, baseWidth), parent.width - 24) : implicitWidth
	height: parent ? Math.min(implicitHeight, parent.height - 24) : implicitWidth
	modal: true
	focus: true

	function deltaY() {
		var unscaled = Qt.inputMethod.keyboardRectangle.height / Screen.devicePixelRatio;
		var availHeight = (parent.height + extraHeight) - unscaled - 24; //margins
		var rawDelta = (Math.max(0, availHeight - height) / 2);
		return rawDelta + 12 - extraHeight; //spacing
	}
}

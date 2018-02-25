import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3

ToolButton {
	id: _toolButton

	property alias source: _tintIcon.source
	property alias toolTip: _backToolTip.text

	implicitHeight: 56.0
	implicitWidth: 56.0

	contentItem: TintIcon {
		id: _tintIcon
		implicitHeight: _toolButton.implicitHeight
		implicitWidth: _toolButton.implicitWidth
	}

	ToolTip {
		id: _backToolTip
		Material.foreground: "#FFFFFF"
	}

	onPressAndHold: {
		//TODO QuickExtras.hapticLongPress();
		_backToolTip.visible = true;
	}
	onCanceled: _backToolTip.visible = false
	onReleased: _backToolTip.visible = false
}

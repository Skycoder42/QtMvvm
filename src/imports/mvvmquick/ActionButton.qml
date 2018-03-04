import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import de.skycoder42.QtMvvm.Quick 1.0

ToolButton {
	id: _toolButton

	property string toolTip: _toolButton.text

	display: AbstractButton.IconOnly

	implicitHeight: 48
	implicitWidth: 48

	icon.width: 24
	icon.height: 24

	ToolTip {
		id: _backToolTip
		text: _toolButton.toolTip
	}

	onPressAndHold: {
		QuickPresenter.hapticLongPress();
		_backToolTip.visible = true;
	}
	onCanceled: _backToolTip.visible = false
	onReleased: _backToolTip.visible = false
}

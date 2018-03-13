import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.QtMvvm.Quick 1.0

/*! @brief An extension of the @ref QtQuick.Controls.RoundButton "RoundButton" for better appearance
 *
 * @extends QtQuick.Controls.RoundButton
 *
 * @details This version basically adjusts size, icon size and text display to look better and
 * fit the Material guidelines. It also adds a tooltip that can be shown via a long press
 *
 * @sa ContrastToolBar, MenuButton
 */
RoundButton {
	id: _roundButton

	//! @copydoc ActionButton
	property string toolTip: _roundButton.text

	display: AbstractButton.IconOnly
	highlighted: true

	implicitHeight: 56 + padding
	implicitWidth: 56 + padding

	icon.width: 24
	icon.height: 24

	ToolTip {
		id: _backToolTip
		text: _roundButton.toolTip
	}

	onPressAndHold: {
		if(_backToolTip.text !== "") {
			QuickPresenter.hapticLongPress();
			_backToolTip.visible = true;
		}
	}
	onCanceled: _backToolTip.visible = false
	onReleased: _backToolTip.visible = false
}

import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.QtMvvm.Quick 1.1

/*! @brief An extension of the @ref QtQuick.Controls.ToolButton "ToolButton" for better appearance
 *
 * @extends QtQuick.Controls.ToolButton
 *
 * @details This version basically adjusts size, icon size and text display to look better and
 * fit the Material guidelines. It also adds a tooltip that can be shown via a long press
 *
 * @sa ContrastToolBar, MenuButton
 */
ToolButton {
	id: _toolButton

	/*! @brief A toolTip to be shown when the button is long pressed
	 *
	 * @default{`ToolButton.text` (binding)}
	 *
	 * @accessors{
	 *	@memberAc{toolTip}
	 *  @notifyAc{toolTipChanged()}
	 * }
	 */
	property string toolTip: _toolButton.text

	display: AbstractButton.IconOnly

	implicitHeight: 48
	//implicitWidth: 48

	icon.width: 24
	icon.height: 24

	ColorHelper {
		id: helper
	}
	icon.color: helper.text

	ToolTip.visible: pressed && _toolButton.toolTip != ""
	ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
	ToolTip.text: _toolButton.toolTip

	onPressAndHold: {
		if(_toolButton.toolTip !== "")
			QuickPresenter.hapticLongPress();
	}
}

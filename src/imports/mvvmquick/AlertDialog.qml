import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Window 2.2

/*! @brief An extension of the @ref QtQuick.Controls.Dialog "Dialog" for better appearance
 *
 * @extends QtQuick.Controls.Dialog
 *
 * @details This version basically adjusts size and makes sure the dialog always appears in
 * center of the application. It even takes a possible input method into accout
 */
Dialog {
	id: _alertDialog

	/*! @brief Additional hight to give to the dialog
	 *
	 * @default{`0`}
	 *
	 * The dialog basically assumes that the parent is it's normal height plus this value. The
	 * combined height is then used to calculate where to show the dialog. Can be used if the
	 * parent does not fill up the whole application.
	 *
	 * @accessors{
	 *	@memberAc{extraHeight}
	 *  @notifyAc{extraHeightChanged()}
	 * }
	 */
	property real extraHeight: 0
	/*! @brief The ideal basic width of the dialog
	 *
	 * @default{`300`}
	 *
	 * Typically, the dialogs width is deduces by using the implicitWidth of it's contents and
	 * is limited to the parents width. Howver some contents have no useful implicit width.
	 * With this property, the dialog will try to be at least `baseWidth` or the contents
	 * implicitWidth wide, whichever is bigger.
	 *
	 * @accessors{
	 *	@memberAc{baseWidth}
	 *  @notifyAc{baseWidthChanged()}
	 * }
	 */
	property real baseWidth: 300

	x: parent ? (parent.width - width) / 2 : 0
	y: parent ? deltaY() : 0
	width: parent ? Math.min(Math.max(implicitWidth, baseWidth), parent.width - 24) : implicitWidth
	height: parent ? Math.min(implicitHeight, parent.height - 24) : implicitWidth
	modal: true
	focus: true

	/*! @brief A function that calculates the value for the y coordiante
	 *
	 * @return type:real A value to be used as the `y` property value
	 *
	 * The value is calculated by taking the parent heigth, the dialog height, the input method
	 * height and the extraHeight into account. I is calculated to center the dialog in the
	 * parent.
	 *
	 * @sa Dialog::extraHeight
	 */
	function deltaY() {
		var unscaled = Qt.inputMethod.keyboardRectangle.height / Screen.devicePixelRatio;
		var availHeight = (parent.height + extraHeight) - unscaled - 24; //margins
		var rawDelta = (Math.max(0, availHeight - height) / 2);
		return rawDelta + 12 - extraHeight; //spacing
	}
}

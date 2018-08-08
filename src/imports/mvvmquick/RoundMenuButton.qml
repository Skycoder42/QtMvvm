import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.QtMvvm.Quick 1.1

/*! @brief An extension of the RoundActionButton to provide a roudn button with sub-elements
 *
 * @extends QtQuick.Item
 *
 * The elment consists of a primary round button. When clicked, the button gets toggled and
 * moves out smaller round buttons from it in a row with the given menu actions on them. The
 * sub buttons are created from a list of @ref QtQuick.Controls.Action "Action" elements
 *
 * @code{.qml}
 * RoundMenuButton {
 *		id: mButton
 *
 *		Action {
 *			text: "Copy"
 *			icon.name: "copy"
 *		}
 *
 *		Action {
 *			text: "Paste"
 *			icon.name: "paste"
 *		}
 *
 *		//...
 * }
 * @endcode
 *
 * @sa RoundActionButton, MenuButton
 */
Item {
	id: _roundMenuButton

	/*! @brief type:string The text/tooltip to be shown for the primary button
	 *
	 * @default{<i>empty</i>}
	 *
	 * @accessors{
	 *	@memberAc{text}
	 *  @notifyAc{textChanged()}
	 * }
	 */
	property alias text: _rootButton.text
	/*! @brief type:Icon The icon to be shown for the primary button
	 *
	 * @default{<i>empty</i>}
	 *
	 * @accessors{
	 *	@memberAc{icon}
	 *  @notifyAc{iconChanged()}
	 * }
	 */
	property alias icon: _rootButton.icon
	/*! @brief type:bool The current check state of the primary button
	 *
	 * @default{`false`}
	 *
	 * If set to true, the button is checked and the sub buttons visible. If false, the button
	 * is not checked and the sub buttons are hidden.
	 *
	 * @accessors{
	 *	@memberAc{checked}
	 *  @notifyAc{checkedChanged()}
	 * }
	 */
	property alias checked: _rootButton.checked

	/*! @brief The spacing between individual sub buttons
	 *
	 * @default{`16`}
	 *
	 * @accessors{
	 *	@memberAc{buttonSpacing}
	 *  @notifyAc{buttonSpacingChanged()}
	 * }
	 */
	property real buttonSpacing: 16
	/*! @brief The angle (in degree) to show the sub buttons in
	 *
	 * @default{`0`}
	 *
	 * The sub buttons are expanded in a line from the center of the primary buttons. The
	 * direction in wich they expand is determined by this angle. An angle of 0° means they
	 * are displayed straight above the button. From there, the angle is counted clockwise,
	 * i.e. an angle of 90° will show them straight to the right
	 *
	 * @accessors{
	 *	@memberAc{expansionAngle}
	 *  @notifyAc{expansionAngleChanged()}
	 * }
	 */
	property real expansionAngle: 0
	/*! @brief The size of the sub buttons
	 *
	 * @default{`Qt.size(40, 40)`}
	 *
	 * @accessors{
	 *	@memberAc{subButtonSize}
	 *  @notifyAc{subButtonSizeChanged()}
	 * }
	 */
	property size subButtonSize: Qt.size(40, 40)
	/*! @brief Tooltips are automatically shown beneath each sub button
	 *
	 * @default{`false`}
	 *
	 * Normally, the button tooltips are only visible when the sub buttons are long pressed.
	 * Setting this to true will automatically show the tooltips on expansion and keep them
	 * visible as long as the items are expanded.
	 *
	 * @accessors{
	 *	@memberAc{stickyToolTips}
	 *  @notifyAc{stickyToolTipsChanged()}
	 * }
	 *
	 * @sa RoundMenuButton::invertToolTipDirection
	 */
	property bool stickyToolTips: false
	/*! @brief Can be used to inverte the tooltip position
	 *
	 * @default{`Qt.application.layoutDirection == Qt.RightToLeft`}
	 *
	 * The position of the sticky tooltips is determined automatically depending on the
	 * expansionAngle. It can be to the right or to the left of the buttons. In case you need
	 * to show them on the exact opposite size, you can set this property to true
	 *
	 * @accessors{
	 *	@memberAc{invertToolTipDirection}
	 *  @notifyAc{invertToolTipDirectionChanged()}
	 * }
	 *
	 * @sa RoundMenuButton::stickyToolTips, RoundMenuButton::expansionAngle
	 */
	property bool invertToolTipDirection: Qt.application.layoutDirection == Qt.RightToLeft

	/*! @brief type:RoundActionButton A reference to the primary button
	 *
	 * @default{<i>A RoundActionButton instance</i>}
	 *
	 * @accessors{
	 *	@memberAc{rootButton}
	 *  @readonlyAc
	 * }
	 */
	readonly property alias rootButton: _rootButton

	/*! @brief The actions that are displayed as the sub buttons
	 *
	 * @default{<i>empty</i>}
	 *
	 * @accessors{
	 *	@memberAc{actions}
	 *  @defaultAc
	 * }
	 */
	default property list<Action> actions

	implicitWidth: _rootButton.implicitWidth
	implicitHeight: _rootButton.implicitHeight

	RoundActionButton {
		id: _rootButton
		z: 10
		anchors.fill: parent
		checkable: true
	}

	QtObject {
		id: _p

		function toRadians(angle) {
		  return angle * (Math.PI/180);
		}

		readonly property real vOffset: -1 * Math.cos(toRadians(_roundMenuButton.expansionAngle))
		readonly property real hOffset: Math.sin(toRadians(_roundMenuButton.expansionAngle))
	}

	Repeater {
		model: actions

		delegate: RoundActionButton {
			id: _subButton

			highlighted: false
			anchors.horizontalCenter: _rootButton.horizontalCenter
			anchors.verticalCenter: _rootButton.verticalCenter
			implicitHeight: _roundMenuButton.subButtonSize.height + padding
			implicitWidth: _roundMenuButton.subButtonSize.width + padding
			state: _rootButton.checked ? "expanded" : "collapsed"

			toolTip: _roundMenuButton.stickyToolTips ? "" : _subButton.text

			onClicked: _rootButton.checked = false

			/* calc offsets as:
			 * - one subbutton (from center to center) + spacing between buttons
			 * - that times the number of buttons before this one + +1
			 * - that plus the extra height delta from the root button
			 * - that times the factor from the angle
			 */
			readonly property real maxVOffset: _p.vOffset * ((1 + index) * (_subButton.height + _roundMenuButton.buttonSpacing) + (_rootButton.height - _subButton.height)/2)
			readonly property real maxHOffset: _p.hOffset * ((1 + index) * (_subButton.width + _roundMenuButton.buttonSpacing) + (_rootButton.width - _subButton.width)/2)

			action: modelData

			ToolTip {
				id: _permaToolTip
				visible: _roundMenuButton.stickyToolTips && _subButton.text != "" && _subButton.visible
				text: _subButton.text
				x: invertToolTipDirection ?
					   _subButton.width + _roundMenuButton.buttonSpacing :
					   -(_permaToolTip.width + _roundMenuButton.buttonSpacing)
				y: (_subButton.height - height)/2
			}

			states: [
				State {
					name: "collapsed"
					PropertyChanges {
						target: _subButton
						anchors.verticalCenterOffset: 0
						anchors.horizontalCenterOffset: 0
						visible: false
					}
				},
				State {
					name: "expanded"
					PropertyChanges {
						target: _subButton
						anchors.verticalCenterOffset: maxVOffset
						anchors.horizontalCenterOffset: maxHOffset
						visible: true
					}
				}
			]

			transitions: [
				Transition {
					from: "collapsed"
					to: "expanded"
					SequentialAnimation {
						PropertyAnimation {
							target: _subButton
							property: "visible"
							duration: 0
						}
						ParallelAnimation {
							PropertyAnimation {
								target: _subButton
								property: "anchors.verticalCenterOffset"
								duration: 250
								easing.type: Easing.OutCubic
							}
							PropertyAnimation {
								target: _subButton
								property: "anchors.horizontalCenterOffset"
								duration: 250
								easing.type: Easing.OutCubic
							}
						}
					}
				},
				Transition {
					from: "expanded"
					to: "collapsed"
					SequentialAnimation {
						ParallelAnimation {
							PropertyAnimation {
								target: _subButton
								property: "anchors.verticalCenterOffset"
								duration: 250
								easing.type: Easing.InCubic
							}
							PropertyAnimation {
								target: _subButton
								property: "anchors.horizontalCenterOffset"
								duration: 250
								easing.type: Easing.InCubic
							}
						}
						PropertyAnimation {
							target: _subButton
							property: "visible"
							duration: 0
						}
					}
				}
			]
		}
	}
}

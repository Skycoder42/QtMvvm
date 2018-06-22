import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.QtMvvm.Quick 1.1

Item {
	id: _roundMenuButton

	property alias text: _rootButton.text
	property alias icon: _rootButton.icon
	property alias checked: _rootButton.checked

	property real buttonSpacing: 16
	property real expansionAngle: 0
	property size subButtonSize: Qt.size(40, 40)
	property bool stickyToolTips: false
	property bool invertToolTipDirection: Qt.application.layoutDirection == Qt.RightToLeft

	readonly property alias rootButton: _rootButton

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

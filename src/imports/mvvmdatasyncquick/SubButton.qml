import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.QtMvvm.Quick 1.0

RoundActionButton {
	id: _subButton

	property Item reference: parent
	property real btnSpacing: 16
	property bool expanded: false

	highlighted: false
	anchors.horizontalCenter: reference.horizontalCenter
	anchors.verticalCenter: reference.verticalCenter
	implicitHeight: 40 + padding
	implicitWidth: 40 + padding
	state: expanded ? "expanded" : "collapsed"

	states: [
		State {
			name: "collapsed"
			PropertyChanges {
				target: _subButton
				anchors.verticalCenterOffset: 0
				visible: false
			}
		},
		State {
			name: "expanded"
			PropertyChanges {
				target: _subButton
				anchors.verticalCenterOffset: -1 * (reference.height/2 + _subButton.height/2 + btnSpacing)
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
				PropertyAnimation {
					target: _subButton
					property: "anchors.verticalCenterOffset"
					duration: 250
					easing.type: Easing.OutCubic
				}
			}
		},
		Transition {
			from: "expanded"
			to: "collapsed"
			SequentialAnimation {
				PropertyAnimation {
					target: _subButton
					property: "anchors.verticalCenterOffset"
					duration: 250
					easing.type: Easing.InCubic
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

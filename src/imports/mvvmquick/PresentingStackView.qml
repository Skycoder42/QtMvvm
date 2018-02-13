import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.qtmvvm.quick 1.0

StackView {
	id: mainStack
	anchors.fill: parent

	readonly property int animDuration: 150
	readonly property int opDuration: 75

	function presentItem(item) {
		if(push(item)) {
			QuickPresenter.qmlPresenter.opened(item);
			return true;
		} else
			return false;
	}

	function withdrawItem(item) {
		if(currentItem === item) {
			if(pop()) {
				QuickPresenter.qmlPresenter.closed(item);
				return true;
			}
		}

		return false;
	}

	function closeAction() {
		if(typeof mainStack.currentItem.closeAction != "undefined") {
			if(mainStack.currentItem.closeAction())
				return true;
		}

		if(mainStack.depth <= 1)
			return false;
		else {
			var item = mainStack.pop();
			if(item) {
				QuickPresenter.qmlPresenter.closed(item);
				return true;
			} else
				return false;
		}
	}

	pushEnter: Transition {
		PropertyAnimation {
			property: "y"
			easing.type: Easing.InOutQuad
			from: height * 0.3
			to: 0
			duration: mainStack.animDuration
		}
		PropertyAnimation {
			property: "opacity"
			from: 0.0
			to: 1.0
			duration: mainStack.opDuration
		}
	}
	pushExit: Transition {
		PauseAnimation {
			duration: mainStack.animDuration
		}
	}
	popEnter: Transition {
		PauseAnimation {
			duration: mainStack.animDuration
		}
	}
	popExit: Transition {
		PropertyAnimation {
			property: "y"
			easing.type: Easing.InOutQuad
			from: 0
			to: height * 0.3
			duration: mainStack.animDuration
		}
		SequentialAnimation {
			PauseAnimation {
				duration: mainStack.animDuration - mainStack.opDuration
			}
			PropertyAnimation {
				id: pp1
				property: "opacity"
				from: 1.0
				to: 0.0
				duration: mainStack.opDuration
			}
		}
	}
}

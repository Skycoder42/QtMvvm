import QtQuick 2.10
import QtQuick.Controls 2.3

StackView {
	id: _presenterStack

	property int animDuration: 150
	property int opDuration: 75

	property var _clearItems: []

	function presentItem(item) {
		if(item.presentAsRoot) { //TODO document
			if(safeReplace(null, item))
				return true;
			else
				return false;
		} else {
			if(push(item))
				return true;
			else
				return false;
		}
	}

	function closeAction() {
		if(typeof _presenterStack.currentItem.closeAction == "function") {
			if(_presenterStack.currentItem.closeAction())
				return true;
		}

		if(_presenterStack.depth <= 1)
			return false;
		else {
			if(_presenterStack.safePop())
				return true;
			else
				return false;
		}
	}

	function safePop(item, operation) {
		var resItem = pop(item, operation)
		if(resItem) {
			_clearItems.push(resItem);
			return true;
		} else
			return false;
	}

	function safeReplace(target, item, properties, operation) {
		var items = [];
		console.log("current depth: ", depth)
		for(var i = depth -1; i >= 0; i--) {
			var cItem = get(i, StackView.ForceLoad);
			_clearItems.push(cItem);
			if(cItem === target)
				break;
		}
		if(replace(target, item, properties, operation))
			return true;
		else
			return false;
	}

	function clearWaitingItems() {
		_clearItems.forEach(function(item) {
			item.destroy();
		});
		_clearItems = [];
	}

	//TODO only for android? maybe move to second class?
	pushEnter: Transition {
		PropertyAnimation {
			property: "y"
			easing.type: Easing.InOutQuad
			from: height * 0.3
			to: 0
			duration: _presenterStack.animDuration
		}
		PropertyAnimation {
			property: "opacity"
			from: 0.0
			to: 1.0
			duration: _presenterStack.opDuration
		}
	}
	pushExit: Transition {
		PauseAnimation {
			duration: _presenterStack.animDuration
		}

		onRunningChanged: {
			if(!running)
				Qt.callLater(clearWaitingItems)
		}
	}
	popEnter: Transition {
		PauseAnimation {
			duration: _presenterStack.animDuration
		}
	}
	popExit: Transition {
		PropertyAnimation {
			property: "y"
			easing.type: Easing.InOutQuad
			from: 0
			to: height * 0.3
			duration: _presenterStack.animDuration
		}
		SequentialAnimation {
			PauseAnimation {
				duration: _presenterStack.animDuration - _presenterStack.opDuration
			}
			PropertyAnimation {
				id: pp1
				property: "opacity"
				from: 1.0
				to: 0.0
				duration: _presenterStack.opDuration
			}
		}

		onRunningChanged: {
			if(!running)
				Qt.callLater(clearWaitingItems)
		}
	}
	replaceEnter: pushEnter
	replaceExit: pushExit
}

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

RowLayout {
	id: _edit
	spacing: 0

	property date time: new Date()
	onTimeChanged: {
		if(!_skipChange)
			_forceTime = time
	}

	property var _forceTime: new Date()

	readonly property bool is24Hours: {
		var fmStr = Qt.locale().timeFormat(Locale.LongFormat);
		var isApPm = false;
		["A", "AP", "a", "ap"].forEach(function(text){
			if(fmStr.indexOf(text) !== -1)
				isApPm = true;
		});
		return !isApPm;
	}

	readonly property int hours: _hourTumbler.currentIndex + (!is24Hours && _amPmTumbler.currentIndex === 1 ? 12 : 0);
	readonly property int minutes: _minuteTumbler.currentIndex;
	property bool _skipChange: false

	function recalcTime() {
		var hours = _hourTumbler.currentIndex + (!is24Hours && _amPmTumbler.currentIndex === 1 ? 12 : 0);
		var minutes = _minuteTumbler.currentIndex;
		if(_forceTime.getHours() !== hours ||
		   _forceTime.getMinutes() !== minutes) {
			_skipChange = true;
			time = new Date(0, 0, 0, hours, minutes);
			_skipChange = false;
		}
	}

	Item {
		id: _spacer1
		Layout.fillHeight: true
		Layout.fillWidth: true
		Layout.preferredWidth: 0
	}

	Tumbler {
		id: _hourTumbler
		model: {
			var model = new Array(is24Hours ? 24 : 12);
			for(var i = 0; i < model.length; i++)
				model[i] = i.toString();
			if(!is24Hours)
				model[0] = 12;
			return model;
		}
		currentIndex: is24Hours ? _forceTime.getHours() : (_forceTime.getHours() % 12)
		onCurrentIndexChanged: Qt.callLater(recalcTime)
	}

	Label {
		text: qsTr(":")
		Layout.minimumWidth: implicitWidth
	}

	Tumbler {
		id: _minuteTumbler
		model: {
			var mod = new Array(60)
			for(var i = 0; i < mod.length; i++) {
				var data = i.toString();
				mod[i] = data.length < 2 ? Qt.locale().zeroDigit + data : data;
			}
			return mod;
		}
		currentIndex: _forceTime.getMinutes()
		onCurrentIndexChanged: Qt.callLater(recalcTime)
	}

	Tumbler {
		id: _amPmTumbler
		visible: !is24Hours
		model: [
			Qt.locale().amText,
			Qt.locale().pmText,
		]
		currentIndex: _forceTime.getHours() >= 12 ? 1 : 0
		onCurrentIndexChanged: Qt.callLater(recalcTime)
	}

	Item {
		id: _spacer2
		Layout.fillHeight: true
		Layout.fillWidth: true
		Layout.preferredWidth: 0
	}
}

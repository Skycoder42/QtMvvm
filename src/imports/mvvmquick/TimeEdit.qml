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

	property bool showHours: true
	property bool showMinutes: true
	property bool showSeconds: false

	readonly property bool is24Hours: {
		var fmStr = Qt.locale().timeFormat(Locale.LongFormat);
		var isApPm = false;
		["A", "AP", "a", "ap"].forEach(function(text){
			if(fmStr.indexOf(text) !== -1)
				isApPm = true;
		});
		return !isApPm;
	}

	readonly property int hours: showHours ? (_hourTumbler.currentIndex + (!is24Hours && _amPmTumbler.currentIndex === 1 ? 12 : 0)) : 0
	readonly property int minutes: showMinutes ? _minuteTumbler.currentIndex : 0
	readonly property int seconds: showSeconds ? _secondsTumbler.currentIndex : 0
	property bool _skipChange: false

	function recalcTime() {
		if(_forceTime.getHours() !== hours ||
		   _forceTime.getMinutes() !== minutes ||
		   _forceTime.getSeconds() !== seconds) {
			_skipChange = true;
			time = new Date(0, 0, 0, hours, minutes, seconds);
			_skipChange = false;
		}
	}

	Item {
		id: _spacer1
		Layout.fillHeight: true
		Layout.fillWidth: true
		Layout.preferredWidth: 0
	}

	TimeTumbler {
		id: _hourTumbler
		visible: showHours
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
		visible: showHours && showMinutes
	}

	TimeTumbler {
		id: _minuteTumbler
		visible: showMinutes
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

	Label {
		text: qsTr(":")
		Layout.minimumWidth: implicitWidth
		visible: showSeconds && (showMinutes || showHours)
	}

	TimeTumbler {
		id: _secondsTumbler
		visible: showSeconds
		model: {
			var mod = new Array(60)
			for(var i = 0; i < mod.length; i++) {
				var data = i.toString();
				mod[i] = data.length < 2 ? Qt.locale().zeroDigit + data : data;
			}
			return mod;
		}
		currentIndex: _forceTime.getSeconds()
		onCurrentIndexChanged: Qt.callLater(recalcTime)
	}

	TimeTumbler {
		id: _amPmTumbler
		visible: !is24Hours && showHours
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

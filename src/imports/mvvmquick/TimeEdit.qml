import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

/*! @brief A edit view to edit times by using tumblers
 *
 * @extends QtQuick.Layouts.RowLayout
 *
 * You can use this edit if you want to let the user select a time. You can use the properties
 * to control wich tumblers are shown and which format they use
 */
RowLayout {
	id: _edit
	spacing: 0

	/*! @brief The time currently displayed and edited
	 *
	 * @default{`new Date()`}
	 *
	 * @note Only the time-componente of the date object is used.
	 *
	 * @accessors{
	 *	@memberAc{time}
	 *  @notifyAc{timeChanged()}
	 * }
	 */
	property date time: new Date()
	onTimeChanged: {
		if(!_p.skipChange)
			_p.forceTime = time
	}

	/*! @brief Specify, whether the user should be able to edit the hours of the time
	 *
	 * @default{`true`}
	 *
	 * @accessors{
	 *	@memberAc{showHours}
	 *  @notifyAc{showHoursChanged()}
	 * }
	 */
	property bool showHours: true
	/*! @brief Specify, whether the user should be able to edit the minutes of the time
	 *
	 * @default{`true`}
	 *
	 * @accessors{
	 *	@memberAc{showMinutes}
	 *  @notifyAc{showMinutesChanged()}
	 * }
	 */
	property bool showMinutes: true
	/*! @brief Specify, whether the user should be able to edit the seconds of the time
	 *
	 * @default{`false`}
	 *
	 * @accessors{
	 *	@memberAc{showSeconds}
	 *  @notifyAc{showSecondsChanged()}
	 * }
	 */
	property bool showSeconds: false

	/*! @brief Holds the time format to use to edit the time
	 *
	 * @default{<i>locale dependend</i>}
	 *
	 * In 24h format, the hour tumbler shows 0-24 as numbers. In the 12h format, it shows the
	 * numbers 1-12 and an additional tumbler with "am" and "pm" is shown to the far right.
	 * The correct version is detected from the locale, but you can always overwrite it to fit
	 * your needs.
	 *
	 * @accessors{
	 *	@memberAc{use24HourFormat}
	 *  @notifyAc{use24HourFormatChanged()}
	 * }
	 */
	property bool use24HourFormat: {
		var fmStr = Qt.locale().timeFormat(Locale.LongFormat);
		var isApPm = false;
		["A", "AP", "a", "ap"].forEach(function(text){
			if(fmStr.indexOf(text) !== -1)
				isApPm = true;
		});
		return !isApPm;
	}

	/*! @brief Holds the currently displayed hours (in 24h format)
	 *
	 * @default{<i>time dependend</i>}
	 *
	 * @accessors{
	 *	@memberAc{hours}
	 *  @notifyAc{hoursChanged()}
	 *	@readonlyAc
	 * }
	 */
	readonly property int hours: showHours ? (_hourTumbler.currentIndex + (!use24HourFormat && _amPmTumbler.currentIndex === 1 ? 12 : 0)) : 0
	/*! @brief Holds the currently displayed minutes
	 *
	 * @default{<i>time dependend</i>}
	 *
	 * @accessors{
	 *	@memberAc{minutes}
	 *  @notifyAc{minutesChanged()}
	 *	@readonlyAc
	 * }
	 */
	readonly property int minutes: showMinutes ? _minuteTumbler.currentIndex : 0
	/*! @brief Holds the currently displayed seconds
	 *
	 * @default{<i>time dependend</i>}
	 *
	 * @accessors{
	 *	@memberAc{seconds}
	 *  @notifyAc{secondsChanged()}
	 *	@readonlyAc
	 * }
	 */
	readonly property int seconds: showSeconds ? _secondsTumbler.currentIndex : 0

	QtObject {
		id: _p
		property date forceTime: new Date()
		property bool skipChange: false

		function recalcTime() {
			if(_p.forceTime.getHours() !== hours ||
			   _p.forceTime.getMinutes() !== minutes ||
			   _p.forceTime.getSeconds() !== seconds) {
				_p.skipChange = true;
				time = new Date(0, 0, 0, hours, minutes, seconds);
				_p.skipChange = false;
			}
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
			var model = new Array(use24HourFormat ? 24 : 12);
			for(var i = 0; i < model.length; i++)
				model[i] = i.toString();
			if(!use24HourFormat)
				model[0] = 12;
			return model;
		}
		currentIndex: use24HourFormat ? _p.forceTime.getHours() : (_p.forceTime.getHours() % 12)
		onCurrentIndexChanged: Qt.callLater(_p.recalcTime)
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
		currentIndex: _p.forceTime.getMinutes()
		onCurrentIndexChanged: Qt.callLater(_p.recalcTime)
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
		currentIndex: _p.forceTime.getSeconds()
		onCurrentIndexChanged: Qt.callLater(_p.recalcTime)
	}

	TimeTumbler {
		id: _amPmTumbler
		visible: !use24HourFormat && showHours
		model: [
			Qt.locale().amText,
			Qt.locale().pmText,
		]
		currentIndex: _p.forceTime.getHours() >= 12 ? 1 : 0
		onCurrentIndexChanged: Qt.callLater(_p.recalcTime)
	}

	Item {
		id: _spacer2
		Layout.fillHeight: true
		Layout.fillWidth: true
		Layout.preferredWidth: 0
	}
}

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Quick 1.1 as QtMvvm

QtMvvm.TimeEdit {
	id: _edit

	property var inputValue: new Date()

	onInputValueChanged: {
		var realTime = typeof inputValue == "string" ? new Date("2000-01-01T" + inputValue) : inputValue
		if(realTime.getHours() !== time.getHours() ||
		   realTime.getMinutes() !== time.getMinutes()) {
			time = realTime
		}
	}
	onTimeChanged: inputValue = time
}

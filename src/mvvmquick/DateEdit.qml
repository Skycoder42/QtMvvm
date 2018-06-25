import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Quick 1.1 as QtMvvm

QtMvvm.DateEdit {
	id: _edit

	property var inputValue: new Date()

	onInputValueChanged: {
		var realDate = typeof inputValue == "string" ? new Date(inputValue + "T00:00:00") : inputValue
		if(realDate.getDate() !== currentDate.getDate() ||
		   realDate.getMonth() !== currentDate.getMonth() ||
		   realDate.getYear() !== currentDate.getYear()) {
			currentDate = realDate
		}
	}
	onCurrentDateChanged: inputValue = currentDate
}

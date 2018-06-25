import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Quick 1.1 as QtMvvm

ColumnLayout {
	id: _edit

	property var inputValue: new Date()

	readonly property date combinedDate: new Date(_dateEdit.currentDate.getFullYear(),
												  _dateEdit.currentDate.getMonth(),
												  _dateEdit.currentDate.getDate(),
												  _timeEdit.time.getHours(),
												  _timeEdit.time.getMinutes(),
												  _timeEdit.time.getSeconds())

	onInputValueChanged: {
		var realDate = typeof inputValue == "string" ? new Date(inputValue) : inputValue
		if(realDate.getYear() !== combinedDate.getYear() ||
		   realDate.getMonth() !== combinedDate.getMonth() ||
		   realDate.getDate() !== combinedDate.getDate() ||
		   realDate.getHours() !== combinedDate.getHours() ||
		   realDate.getMinutes() !== combinedDate.getMinutes() ||
		   realDate.getSeconds() !== combinedDate.getSeconds()) {
			_dateEdit.currentDate = realDate;
			_timeEdit.time = realDate;
		}
	}
	onCombinedDateChanged: {
		inputValue = combinedDate
	}

	TabBar {
		id: _tabBar
		position: TabBar.Header
		currentIndex: _swipeView.currentIndex
		Layout.fillWidth: true

		Material.background: "transparent"
		Universal.background: "transparent"

		TabButton {
			text: qsTr("Date")
		}

		TabButton {
			text: qsTr("Time")
		}
	}

	SwipeView {
		id: _swipeView
		currentIndex: _tabBar.currentIndex
		Layout.fillWidth: true
		Layout.fillHeight: true
		Layout.preferredHeight: Math.max(_dateEdit.implicitHeight, _timeEdit.implicitHeight)
		Layout.preferredWidth: Math.max(_dateEdit. implicitWidth, _timeEdit. implicitWidth)

		QtMvvm.DateEdit {
			id: _dateEdit
		}

		QtMvvm.TimeEdit {
			id: _timeEdit
		}
	}
}

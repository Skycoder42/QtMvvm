import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Quick 1.1 as QtMvvm

ColumnLayout {
	id: _edit

	property var inputValue: new Date()

	readonly property date combinedDate: new Date(_dateEdit.date.getFullYear(),
												  _dateEdit.date.getMonth(),
												  _dateEdit.date.getDate(),
												  _timeEdit.time.getHours(),
												  _timeEdit.time.getMinutes(),
												  _timeEdit.time.getSeconds())

	onInputValueChanged: {
		var newDate = inputValue;
		if(newDate.getYear() !== combinedDate.getYear() ||
		   newDate.getMonth() !== combinedDate.getMonth() ||
		   newDate.getDate() !== combinedDate.getDate() ||
		   newDate.getHours() !== combinedDate.getHours() ||
		   newDate.getMinutes() !== combinedDate.getMinutes() ||
		   newDate.getSeconds() !== combinedDate.getSeconds()) {
			_dateEdit.date = newDate;
			_timeEdit.time = newDate;
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

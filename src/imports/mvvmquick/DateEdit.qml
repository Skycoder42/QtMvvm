import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import Qt.labs.calendar 1.0
import de.skycoder42.QtMvvm.Quick 1.1

/*! @brief A edit view to edit dates by using a calendar
 *
 * @extends QtQuick.ListView
 *
 * You can use this edit if you want to let the user select a date. You can use the properties
 * to control wich tumblers are shown and which format they use.
 *
 * @note This edit currently uses Qt.labs.calendar - an experimental module, and thus might
 * not always function perfectly or break after Qt updates
 */
ListView {
	id: _calenderList

	/*! @brief The smallest possible date the user can select
	 *
	 * @default{`new Date(1970, 0, 1)`}
	 *
	 * @note Only the date-componente of the date object is used.
	 *
	 * @accessors{
	 *	@memberAc{firstDate}
	 *  @notifyAc{firstDateChanged()}
	 * }
	 */
	property date firstDate: new Date(1970, 0, 1)
	/*! @brief The highest possible date the user can select
	 *
	 * @default{`new Date(9999, 11, 31)`}
	 *
	 * @note Only the date-componente of the date object is used.
	 *
	 * @accessors{
	 *	@memberAc{lastDate}
	 *  @notifyAc{lastDateChanged()}
	 * }
	 */
	property date lastDate: new Date(9999, 11, 31)
	/*! @brief The date currently displayed and edited
	 *
	 * @default{`new Date()`}
	 *
	 * @note Only the date-componente of the date object is used.
	 *
	 * @accessors{
	 *	@memberAc{date}
	 *  @notifyAc{dateChanged()}
	 * }
	 */
	property date date: _p.today();

	QtObject {
		id: _p

		property bool _skipNextFocus: false

		function today() {
			var cDate = new Date();
			cDate.setHours(0, 0, 0, 0, 0);
			return cDate;
		}

		function focusDate() {
			if(_skipNextFocus)
				_skipNextFocus = false;
			else {
				_calenderList.currentIndex = calendarModel.indexOf(_calenderList.date);
				_calenderList.positionViewAtIndex(_calenderList.currentIndex, ListView.SnapPosition);
			}
		}
	}

	Component.onCompleted: _p.focusDate()
	onDateChanged: _p.focusDate()

	implicitWidth: 300
	implicitHeight: 200

	interactive: true
	keyNavigationWraps: true
	snapMode: ListView.SnapOneItem
	orientation: ListView.Horizontal
	highlightRangeMode: ListView.StrictlyEnforceRange
	clip: true

	model: CalendarModel {
		id: calendarModel
		from: _calenderList.firstDate
		to: _calenderList.lastDate
	}

	delegate: GridLayout {
		width: _calenderList.width
		height: _calenderList.height
		columns: 4

		Button {
			Layout.rowSpan: 3
			Layout.fillHeight: true
			Layout.preferredWidth: 36
			flat: true

			icon.name: "go-previous"
			icon.source: "qrc:/de/skycoder42/qtmvvm/quick/icons/ic_navigate_before.svg"
			icon.width: 24
			icon.height: 24

			onClicked: _calenderList.decrementCurrentIndex()
		}

		Label {
			Layout.columnSpan: 2
			Layout.fillWidth: true
			horizontalAlignment: Text.AlignHCenter
			font.bold: true
			text: grid.title
		}

		Button {
			Layout.rowSpan: 3
			Layout.fillHeight: true
			Layout.preferredWidth: 36
			flat: true

			icon.name: "go-next"
			icon.source: "qrc:/de/skycoder42/qtmvvm/quick/icons/ic_navigate_next.svg"
			icon.width: 24
			icon.height: 24

			onClicked: _calenderList.incrementCurrentIndex()
		}

		Item {
			Layout.fillWidth: true
		} // spacer

		DayOfWeekRow {
			Layout.fillWidth: true
			delegate: Label {
				text: model.shortName
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
			}
		}

		WeekNumberColumn {
			month: model.month
			year: model.year
			Layout.fillHeight: true
			delegate: Label {
				text: model.weekNumber
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
			}
		}

		MonthGrid {
			id: grid
			month: model.month
			year: model.year
			Layout.fillWidth: true
			Layout.fillHeight: true

			delegate: Label {
				id: dayDelegate

				ColorHelper {
					id: helper
				}

				readonly property bool isCurrent: model.day === _calenderList.date.getDate() && model.month === _calenderList.date.getMonth()
				readonly property alias highlightColor: helper.highlight

				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
				opacity: model.month === grid.month ? 1 : 0.5
				text: model.day
				font: grid.font
				color: isCurrent ? QuickPresenter.accentTextColor(highlightColor, helper.text) : helper.text

				background: Rectangle {
					readonly property double size: Math.max(dayDelegate.width, dayDelegate.height) * 1.2

					anchors.centerIn: parent
					height: size
					width: size

					color: dayDelegate.highlightColor
					opacity: dayDelegate.isCurrent ? 1 : 0
					radius: size / 2

					Behavior on opacity {
						NumberAnimation {
							duration: 750
							easing.type: Easing.OutBack
						}
					}
				}

				MouseArea {
					anchors.fill: parent

					onClicked: {
						if(model.month < grid.month) {
							_calenderList.decrementCurrentIndex();
							_p._skipNextFocus = true;
						} else if(model.month > grid.month) {
							_calenderList.incrementCurrentIndex();
							_p._skipNextFocus = true;
						}
						_calenderList.date = model.date;
					}
				}
			}
		}

		Item {
			Layout.fillWidth: true
			Layout.columnSpan: 4
			Layout.minimumHeight: 0
			Layout.maximumHeight: 0
		} // spacer, only needed for the additional padding below the month grid
	}
}

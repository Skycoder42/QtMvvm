import QtQuick 2.10
import QtQuick.Controls 2.3

/*! @brief An extension of the @ref QtQuick.Controls.Label "Label" for better appearance when
 * used in a toolbar
 *
 * @extends QtQuick.Controls.Label
 *
 * @details This special label is bigger in size and correctly aligned for use in a toolbar.
 * You can either let it fill the whole toolbar or use it in a layout with
 * `Layout.fillWidth: true` and it will perfectly fill up the bar like a normal toolbar label
 * for mobile activity titles.
 *
 * @sa ContrastToolBar
 */
Label {
	id: _toolLabel
	font.pointSize: 14
	font.bold: true
	elide: Label.ElideRight
	horizontalAlignment: Qt.AlignLeft
	verticalAlignment: Qt.AlignVCenter
	leftPadding: 16
}

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import de.skycoder42.QtMvvm.Quick 1.1

/*! @brief A simply tinted icon, exactly like you find in i.e. the ActionButton
 *
 * @extends QtQuick.Item
 *
 * If you want to use an icon, just like in a ActionButton, you can use this class. It behaves
 * exactly the same, but is of course not clickable.
 *
 * @sa ContrastToolBar, MenuButton
 */
Item {
	id: _tintIcon

	/*! @brief type:Icon The icon itself
	 *
	 * @default{<i>empty icon</i>}
	 *
	 * @accessors{
	 *	@memberAc{icon}
	 *  @notifyAc{iconChanged()}
	 * }
	 *
	 * @sa @ref QtQuick.Controls.AbstractButton "AbstractButton.icon"
	 */
	property alias icon: _imgBtn.icon

	implicitWidth: _imgBtn.icon.width
	implicitHeight: _imgBtn.icon.height

	Displace {
		source: _imgBtn
		z: 10
		anchors.fill: parent
	}

	ActionButton {
		id: _imgBtn
		visible: false
		z: -10
		padding: 0
		anchors.fill: parent

		ColorHelper {
			id: helper
		}
		icon.color: helper.text

		background: Item {}
	}
}

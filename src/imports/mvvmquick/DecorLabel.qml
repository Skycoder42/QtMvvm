import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.QtMvvm.Quick 1.1

/*! @brief An extension of the @ref QtQuick.Controls.Label "Label" for better appearance when
 * used in FormLayouts
 *
 * @extends QtQuick.Controls.Label
 *
 * This label will automatically change it's color to the highlight color if the the edit
 * it is connected to has input focus. This serves as a visual cue to the selected input
 *
 * @sa DecorLabel::edit
 */
Label {
	id: _decorLabel

	ColorHelper {
		id: helper
	}

	/*! @brief The edit this label is connected to
	 *
	 * @default{`nextItemInFocusChain()`}
	 *
	 * If the edit gets the input focus, this label gets highlighted
	 *
	 * @accessors{
	 *	@memberAc{edit}
	 *  @notifyAc{editChanged()}
	 * }
	 */
	property Item edit: _decorLabel.nextItemInFocusChain()

	/*! @brief Checks if the connected edit currently has input focus
	 *
	 * @default{`false`}
	 *
	 * @accessors{
	 *	@memberAc{editHasFocus}
	 *  @notifyAc{editHasFocusChanged()}
	 *	@readonlyAc
	 * }
	 */
	readonly property bool editHasFocus: edit && edit.focus

	color: editHasFocus ? helper.highlight : helper.text
	opacity: editHasFocus ? 1 : 0.5
}

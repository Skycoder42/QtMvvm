import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.QtMvvm.Quick 1.1

Label {
	id: _decorLabel

	ColorHelper {
		id: helper
	}

	property Item edit: _decorLabel.nextItemInFocusChain()

	readonly property bool editHasFocus: edit && edit.focus

	color: editHasFocus ? helper.highlight : helper.text
	opacity: editHasFocus ? 1 : 0.5
}

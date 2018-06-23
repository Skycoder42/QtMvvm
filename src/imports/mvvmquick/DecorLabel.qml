import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import de.skycoder42.QtMvvm.Quick 1.1

Label {
	id: _decorLabel

	property Item edit: _decorLabel.nextItemInFocusChain()

	readonly property bool editHasFocus: edit && edit.focus

	function highlightColor() {
		if(QuickPresenter.currentStyle === "Material")
			return Material.accentColor;
		else if(QuickPresenter.currentStyle === "Universal")
			return Universal.accent;
		else
			return palette.highlight;
	}

	color: editHasFocus ? highlightColor() : palette.text
	opacity: editHasFocus ? 1 : 0.5
}

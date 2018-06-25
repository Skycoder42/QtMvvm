import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.QtMvvm.Quick 1.1

Tumbler {
	id: _timeTumbler

	function highlightColor() {
		if(QuickPresenter.currentStyle === "Material")
			return Material.accentColor;
		else if(QuickPresenter.currentStyle === "Universal")
			return Universal.accent;
		else
			return palette.highlight;
	}

	Rectangle {
		anchors.centerIn: parent
		anchors.verticalCenterOffset: -0.5 * currentItem.height
		width: currentItem.width * 0.8
		height: 1
		color: parent.enabled ? highlightColor() : palette.text
	}

	Rectangle {
		anchors.centerIn: parent
		anchors.verticalCenterOffset: 0.5 * currentItem.height
		width: currentItem.width * 0.8
		height: 1
		color: parent.enabled ? highlightColor() : palette.text
	}
}

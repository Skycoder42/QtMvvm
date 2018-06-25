import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import de.skycoder42.QtMvvm.Quick 1.1

Control {
	id: _colorHelper
	visible: false

	readonly property color highlight: {
		if(QuickPresenter.currentStyle === "Material")
			return Material.accent;
		else if(QuickPresenter.currentStyle === "Universal")
			return Universal.accent;
		else
			return palette.highlight;
	}

	readonly property color text: {
		if(QuickPresenter.currentStyle === "Material")
			return Material.foreground;
		else if(QuickPresenter.currentStyle === "Universal")
			return Universal.foreground;
		else
			return palette.windowText;
	}
}

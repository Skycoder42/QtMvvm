import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3

ToolBar {
	id: _contrastToolBar

	height: 56

	function accentTextColor(accentColor, baseColor) {
		var a = (0.299 * accentColor.r + 0.587 * accentColor.g + 0.144 * accentColor.b);
		if(typeof baseColor !== "undefined") {
			var b = (0.299 * baseColor.r + 0.587 * baseColor.g + 0.144 * baseColor.b);
			if (Math.abs(a - b) >= 0.5)
				return baseColor;
		}
		if(a < 0.5)
			return "#FFFFFF";
		else
			return "#000000";
	}

	Material.foreground: accentTextColor(Material.primary)
}

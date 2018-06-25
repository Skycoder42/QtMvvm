import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3

/*! @brief An extension of the @ref QtQuick.Controls.ToolBar "ToolBar" for better appearance
 *
 * @extends QtQuick.Controls.ToolBar
 *
 * @details This version basically adjusts size and text color of the toolbar itself and
 * controls within the toolbar to look better and improve contrast
 *
 * @sa ActionButton, ToolBarLabel, MenuButton
 */
ToolBar {
	id: _contrastToolBar

	height: 56

	/*! @brief Calculates the optimal text color based on the background color
	 *
	 * @param type:color accentColor The color to find a contrasting text color for
	 * @param type:color baseColor The current default text color
	 * @return type:color A color for text that is easy to read when used with accentColor as
	 * the background
	 *
	 * The method calculates whether the color should be light or dark and then either returns
	 * white or black, depending on which color fits better. If baseColor is specified, then
	 * the color value is checked, too. If baseColor is easily readable, it is simply returned
	 * as result. Otherwise the method proceeds as usual.
	 */
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

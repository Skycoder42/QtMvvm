import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import de.skycoder42.QtMvvm.Quick 1.1

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

	//! @copydoc QuickPresenter::accentTextColor
	function accentTextColor(accentColor, baseColor) {
		return QuickPresenter.accentTextColor(accentColor, baseColor);
	}

	Material.foreground: QuickPresenter.accentTextColor(Material.primary, Material.foreground)
}

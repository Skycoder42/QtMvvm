import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Quick 1.1

/*! @brief A basic input delegate for the QtMvvm::SettingsViewModel
 *
 * @extends QtQuick.Controls.ItemDelegate
 *
 * This delegate will simply display the standard settings properties and a simple value
 * preview text based on the formatter. When pressed, it shows an input dialog for the
 * delegates type to edit the value.
 *
 * You can extend this delegate if you want to have a custom value indicator, but keep the
 * editing mechanism.
 *
 * Available roles are:
 *
 * - group
 * - key
 * - type
 * - title
 * - tooltip
 * - delegateUrl
 * - inputValue
 * - properties
 * - preview
 */
ItemDelegate {
	id: _msgDelegate

	/*! @brief type:Component A @ref QtQml.Component "Component" to be shown as the indicator
	 *
	 * @default{`undefined`}
	 *
	 * @accessors{
	 *	@memberAc{indicatorComponent}
	 *  @notifyAc{indicatorComponentChanged()}
	 * }
	 *
	 * @sa @ref QtQuick.Loader "Loader.sourceComponent"
	 */
	property alias indicatorComponent: _indicator.sourceComponent
	/*! @brief type:url A url to be shown as the indicator
	 *
	 * @default{`undefined`}
	 *
	 * @accessors{
	 *	@memberAc{indicatorSource}
	 *  @notifyAc{indicatorSourceChanged()}
	 * }
	 *
	 * @sa @ref QtQuick.Loader "Loader.source"
	 */
	property alias indicatorSource: _indicator.source

	/*! @brief The type id of the edit dialog to be shown
	 *
	 * @default{`model.type`}
	 *
	 * Normally, the type of the input dialog is the same as the delegates type. But in case
	 * you want to show a different kind of dialog, you can do so by setting this property to
	 * the target type.
	 *
	 * @accessors{
	 *	@memberAc{editDialogType}
	 *  @notifyAc{editDialogTypeChanged()}
	 * }
	 */
	property string editDialogType: type

	//! @private Internal signal
	signal showInput(string key, string title, string type, var defaultValue, var properties);

	text: title

	ToolTip.visible: pressed && tooltip != ""
	ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
	ToolTip.text: tooltip
	onPressAndHold: QuickPresenter.hapticLongPress()

	onClicked: showInput(key, title, editDialogType, inputValue, properties)

	contentItem: GridLayout {
		columns: 2
		rows: 2

		Label {
			id: _titleLabel
			text: _msgDelegate.text
			Layout.row: 0
			Layout.column: 0
			font.bold: true
			elide: Label.ElideRight
			Layout.fillWidth: true
		}

		Label {
			id: _peviewLabel
			visible: preview
			Layout.row: 1
			Layout.column: 0
			text: preview
			wrapMode: Text.WordWrap
			Layout.fillWidth: true
		}

		Loader {
			id: _indicator
			visible: item
			asynchronous: false
			clip: true

			Layout.row: 0
			Layout.column: 1
			Layout.rowSpan: 2
			Layout.minimumWidth: item ? implicitWidth : 0
			Layout.maximumWidth: item ? implicitWidth : 0
		}
	}
}

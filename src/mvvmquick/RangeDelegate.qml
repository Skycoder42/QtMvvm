import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Quick 1.1

ItemDelegate {
	id: _sliderDelegate

	property alias minimum: _slider.from
	property alias maximum: _slider.to
	property string valueFormat: "%L1"

	text: title

	ToolTip.visible: pressed && tooltip != ""
	ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
	ToolTip.text: tooltip
	onPressAndHold: QuickPresenter.hapticLongPress()

	contentItem: ColumnLayout {
		Label {
			id: _titleLabel
			text: _sliderDelegate.text
			font.bold: true
			elide: Label.ElideRight
			Layout.fillWidth: true
		}

		Slider {
			id: _slider
			Layout.fillWidth: true

			stepSize: 1
			snapMode: Slider.SnapAlways
			live: false
			value: inputValue
			onValueChanged: {
				var nValue = Math.round(_slider.value)
				if(nValue != inputValue)
					inputValue = nValue
			}

			ToolTip {
				parent: _slider.handle
				visible: _slider.pressed
				text: valueFormat.arg(Math.round(_slider.from + _slider.visualPosition * (_slider.to - _slider.from)))
			}
		}
	}
}

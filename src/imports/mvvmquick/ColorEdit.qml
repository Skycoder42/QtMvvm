import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import de.skycoder42.QtMvvm.Quick 1.1

/*! @brief A edit view to edit colors by using a color picker
 *
 * @extends QtQuick.Layouts.GridLayout
 *
 * You can use this edit if you want to let the user select a color. It is made out of three
 * sliders to control hue, saturation and value and a TextField to enter a hex color code,
 * next to a small box that previews the color
 */
GridLayout {
	id: _colorPicker
	columns: 2
	rowSpacing: 16

	ColorHelper {
		id: helper
	}

	/*! @brief Specifies whether the alpha channel can be edited
	 *
	 * @default{`false`}
	 *
	 * If set to true, the user can enter a hex color string with an additional alpha value.
	 * The sliders are uneffected from this property
	 *
	 * @accessors{
	 *	@memberAc{alpha}
	 *  @notifyAc{alphaChanged()}
	 * }
	 */
	property bool alpha: false
	/*! @brief The color currently displayed and edited
	 *
	 * @default{`undefined`}
	 *
	 * @accessors{
	 *	@memberAc{color}
	 *  @notifyAc{colorChanged()}
	 * }
	 */
	property color color
	onColorChanged: {
		if(!_p.changing){
			var hsvColor = _p.rgbToHsv(color);
			_p.changing = true;
			hueSlider.value = hsvColor.h;
			saturationSlider.value = 1 - hsvColor.s;
			valueSlider.value = hsvColor.v;
			_colorEdit.setColor(color);
			_p.changing = false;
		}
	}

	QtObject {
		id: _p

		property bool changing: false
		readonly property color color: Qt.hsva(hueSlider.position, 1 - saturationSlider.position, valueSlider.position)
		readonly property color hueColor: Qt.hsva(hueSlider.position, 1, 1);

		onColorChanged: {
			if(!_p.changing) {
				_p.changing = true;
				_colorPicker.color = _p.color
				_colorEdit.setColor(color);
				_p.changing = false;
			}
		}

		function rgbToHsv(rgbColor) {
			var rr, gg, bb,
				r = rgbColor.r,
				g = rgbColor.g,
				b = rgbColor.b,
				h, s,
				v = Math.max(r, g, b),
				diff = v - Math.min(r, g, b),
				diffc = function(c){
					return (v - c) / 6 / diff + 1 / 2;
				};

			if (diff == 0) {
				h = s = 0;
			} else {
				s = diff / v;
				rr = diffc(r);
				gg = diffc(g);
				bb = diffc(b);

				if (r === v) {
					h = bb - gg;
				} else if (g === v) {
					h = (1 / 3) + rr - bb;
				} else if (b === v) {
					h = (2 / 3) + gg - rr;
				}

				if (h < 0) {
					h += 1;
				} else if (h > 1) {
					h -= 1;
				}
			}

			return {
				h: h,
				s: s,
				v: v
			};
		}
	}

	Rectangle {
		id: colorPreview
		color: _colorPicker.color
		Layout.rowSpan: 3
		Layout.preferredWidth: hueSlider.height + saturationSlider.height + valueSlider.height + 2 * _colorPicker.rowSpacing
		Layout.maximumWidth: _colorPicker.width/3
		Layout.fillHeight: true

		border.color: helper.text
		border.width: 1
	}

	Slider {
		id: hueSlider
		Layout.fillWidth: true
		Material.accent: "#FF0000"
		from: 0
		to: 1

		readonly property real handleWidth: 25
		readonly property real handleHeight: ((handleWidth-1)/2)*3 + 1
		leftPadding: (handleWidth-1)/2 + 1
		rightPadding: (handleWidth-1)/2 + 1

		background: LinearGradient {
			x: hueSlider.leftPadding
			y: 0
			implicitWidth: 200
			implicitHeight: hueSlider.handleHeight
			width: hueSlider.availableWidth
			height: implicitHeight
			start: Qt.point(0, 0)
			end: Qt.point(width, 0)

			Rectangle {
				anchors.fill: parent
				color: "transparent"
				border.color: helper.text
			}

			gradient: Gradient {
				GradientStop {
					position: 0.000
					color: Qt.rgba(1, 0, 0, 1)
				}
				GradientStop {
					position: 0.167
					color: Qt.rgba(1, 1, 0, 1)
				}
				GradientStop {
					position: 0.333
					color: Qt.rgba(0, 1, 0, 1)
				}
				GradientStop {
					position: 0.500
					color: Qt.rgba(0, 1, 1, 1)
				}
				GradientStop {
					position: 0.667
					color: Qt.rgba(0, 0, 1, 1)
				}
				GradientStop {
					position: 0.833
					color: Qt.rgba(1, 0, 1, 1)
				}
				GradientStop {
					position: 1.000
					color: Qt.rgba(1, 0, 0, 1)
				}
			}
		}

		handle: Item {
			x: 1 + hueSlider.visualPosition * (hueSlider.availableWidth - 1)
			width: hueSlider.handleWidth
			height: hueSlider.handleHeight

			Rectangle {
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.top: parent.top
				anchors.bottom: circle.top
				width: 1
				color: helper.text
			}

			Rectangle {
				id: circle
				anchors.centerIn: parent
				implicitWidth: parent.width
				implicitHeight: parent.width
				radius: parent.width / 2
				color: Qt.hsva(hueSlider.position, 1, 1)
				border.color: helper.text
			}

			Rectangle {
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.top: circle.bottom
				anchors.bottom: parent.bottom
				width: 1
				color: helper.text
			}
		}
	}

	Slider {
		id: saturationSlider
		Layout.fillWidth: true
		from: 0
		to: 1

		readonly property real handleSize: 24
		leftPadding: handleSize/2 + 1
		rightPadding: handleSize/2 + 1

		background: Item {
			x: saturationSlider.leftPadding
			y: saturationSlider.topPadding + (saturationSlider.availableHeight - height) / 2
			implicitWidth: 200
			implicitHeight: 6
			width: saturationSlider.availableWidth
			height: implicitHeight

			Rectangle {
				rotation: -90
				anchors.centerIn: parent
				width: parent.height
				height: parent.width
				radius: 3
				border.color: helper.text

				gradient: Gradient {
					GradientStop {
						position: 0.000
						color: Qt.hsva(hueSlider.position, 1, 1)
					}
					GradientStop {
						position: 1.000
						color: Qt.hsva(hueSlider.position, 0, 1)
					}
				}
			}
		}

		handle: Rectangle {
			x: 1 + saturationSlider.visualPosition * saturationSlider.availableWidth
			y: saturationSlider.topPadding + (saturationSlider.availableHeight - height)/2
			width: saturationSlider.handleSize
			height: saturationSlider.handleSize
			radius: saturationSlider.handleSize/2
			color: Qt.hsva(hueSlider.position, 1 - saturationSlider.position, 1)
			border.color: helper.text
		}
	}

	Slider {
		id: valueSlider
		Layout.fillWidth: true
		from: 0
		to: 1

		readonly property real handleSize: 24
		leftPadding: handleSize/2 + 1
		rightPadding: handleSize/2 + 1

		background: Item {
			x: valueSlider.leftPadding
			y: valueSlider.topPadding + (valueSlider.availableHeight - height) / 2
			implicitWidth: 200
			implicitHeight: 6
			width: valueSlider.availableWidth
			height: implicitHeight

			Rectangle {
				rotation: -90
				anchors.centerIn: parent
				width: parent.height
				height: parent.width
				radius: 3
				border.color: helper.text

				gradient: Gradient {
					GradientStop {
						position: 0.000
						color: Qt.hsva(hueSlider.position, 1, 0)
					}
					GradientStop {
						position: 1.000
						color: Qt.hsva(hueSlider.position, 1, 1)
					}
				}
			}
		}

		handle: Rectangle {
			x: 1 + valueSlider.visualPosition * valueSlider.availableWidth
			y: valueSlider.topPadding + (valueSlider.availableHeight - height)/2
			width: valueSlider.handleSize
			height: valueSlider.handleSize
			radius: valueSlider.handleSize/2
			color: Qt.hsva(hueSlider.position, 1, valueSlider.position)
			border.color: helper.text
		}
	}

	TextField {
		id: _colorEdit
		Layout.columnSpan: 2
		Layout.fillWidth: true
		selectByMouse: true

		validator: RegExpValidator {
			regExp: _colorPicker.alpha ? /^#(?:[0-9a-f]{4}){1,2}$/ : /^#(?:[0-9a-f]{3}){1,2}$/
		}

		property bool _skipSet: false
		function setColor(color) {
			if(_skipSet)
				_skipSet = false;
			else
				text = color;
		}

		text: _colorPicker.color
		onTextEdited: {
			if(_colorEdit.acceptableInput) {
				_skipSet = true;
				_colorPicker.color = text;
			}
		}
	}
}

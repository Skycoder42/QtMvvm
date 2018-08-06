import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

ApplicationWindow {
	visible: true
	width: 640
	height: 480
	title: qsTr("Android Settings Test")

	Pane {
		anchors.fill: parent

		GridLayout {
			anchors.fill: parent
			columns: 6

			TextField {
				id: keyField
				placeholderText: qsTr("key")
				Layout.fillWidth: true
				Layout.columnSpan: 3
			}

			TextField {
				id: valueField
				placeholderText: qsTr("value")
				Layout.fillWidth: true
				Layout.columnSpan: 3
			}

			Button {
				id: addButton
				text: qsTr("save")
				Layout.fillWidth: true
				Layout.columnSpan: 2
				onClicked: settings.save(keyField.text, valueField.text)
			}

			Button {
				id: loadButton
				text: qsTr("load")
				Layout.fillWidth: true
				Layout.columnSpan: 2
				onClicked: valueField.text = settings.load(keyField.text)
			}

			Button {
				id: removeButton
				text: qsTr("remove")
				Layout.fillWidth: true
				Layout.columnSpan: 2
				onClicked: settings.remove(keyField.text)
			}

			Label {
				id: eventLabel
				Layout.columnSpan: 2
				Layout.fillWidth: true
				Layout.fillHeight: true

				Connections {
					target: settings
					onChangeEvent: eventLabel.text = eventLabel.text + text + "\n";
				}
			}
		}
	}
}

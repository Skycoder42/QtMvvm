import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3
import de.skycoder42.quickextras 2.0

ListView {
	id: listView

	property bool showSections: true

	section.property: showSections ? "category" : ""
	section.labelPositioning: ViewSection.InlineLabels
	section.delegate: ListSection {
		title: section
	}

	delegate: ItemDelegate {
		id: delegate
		width: parent.width

		onClicked: builder.loadSection(settingsSection)

		Timer {
			id: enforcer
			interval: 50
			repeat: false
			running: true

			onTriggered: {
				delegate.implicitHeight = Qt.binding(function(){return grid.implicitHeight + 32});
			}
		}

		contentItem: GridLayout {
			id: grid
			rows: 2
			columns: 2
			columnSpacing: 14

			TintIcon {
				id: tintIcon
				source: icon
				visible: icon != ""
				Layout.row: 0
				Layout.column: 0
				Layout.rowSpan: 2
				Layout.fillHeight: true
				Layout.preferredWidth: iconSize.width
				Layout.preferredHeight: iconSize.height
				Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
			}

			Label {
				id: titleLabel
				text: title
				Layout.row: 0
				Layout.column: 1
				font.bold: true
				elide: Label.ElideRight
				Layout.fillWidth: true
			}

			Label {
				id: textLabel
				visible: tooltip
				Layout.row: 1
				Layout.column: 1
				text: tooltip
				wrapMode: Text.WordWrap
				Layout.fillWidth: true
			}
		}
	}
}

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Quick 1.1

ListView {
	id: _sectionListView

	property bool showSections: true
	property SettingsUiBuilder builder

	section.property: showSections ? "category" : ""
	section.labelPositioning: ViewSection.InlineLabels
	section.delegate: ListSection {
		title: section
	}

	delegate: ItemDelegate {
		id: delegate
		width: parent.width

		onClicked: builder.loadSection(section)

		contentItem: GridLayout {
			id: grid
			rows: 2
			columns: 3
			columnSpacing: 16

			TintIcon {
				id: tintIcon
				icon.source: iconUrl
				visible: iconUrl != ""
				Layout.row: 0
				Layout.column: 0
				Layout.rowSpan: 2
				Layout.fillHeight: true
				Layout.preferredWidth: implicitWidth
				Layout.preferredHeight: implicitHeight
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

			TintIcon {
				id: openIcon
				icon.source: "qrc:/de/skycoder42/qtmvvm/quick/icons/ic_chevron_right.svg"
				Layout.row: 0
				Layout.column: 2
				Layout.rowSpan: 2
				Layout.fillHeight: true
				Layout.preferredWidth: implicitWidth
				Layout.preferredHeight: implicitHeight
				Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
			}
		}
	}
}

import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.QtMvvm.Core 1.0
import de.skycoder42.QtMvvm.Quick 1.0
import de.skycoder42.QtMvvm.Sample 1.0

Pane {
	property TabItemViewModel viewModel: null

	Switch {
		anchors.centerIn: parent
		text: viewModel.title
	}
}

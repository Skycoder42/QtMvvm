import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Core 1.0
import de.skycoder42.QtMvvm.Quick 1.0
import de.skycoder42.quickextras 2.0
import de.skycoder42.QtMvvm.Sample 1.0

Page {
	id: tabView
	property TabViewModel viewModel: null

	header: ActionBar {
		id: bar
		showMenuButton: true
		//showMenuAsBack: true
		title: qsTr("Sample")

		onMenuButtonClicked: QuickPresenter.toggleDrawer()

		tabBar: TabBar {
			id: tabBar
			currentIndex: swipe.currentIndex

			TabButton {
				text: "Test 1"
			}
			TabButton {
				text: "Test 2"
			}
		}
	}

	PresenterProgress {}

	SwipeView {
		id: swipe
		anchors.fill: parent
		currentIndex: bar.tabBarItem.currentIndex

		Pane {
			id: firstPage

			Switch {
				anchors.centerIn: parent
				checked: true
				text: qsTr("Click me!")
			}
		}
		Pane {
			id: secondPage

			Switch {
				anchors.centerIn: parent
				text: qsTr("Click me, too!")
			}
		}
	}
}

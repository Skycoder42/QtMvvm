import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1
import de.skycoder42.QtMvvm.Sample 1.1

Page {
	id: tabView
	property TabViewModel viewModel: null

	header: ContrastToolBar {
		height: 56 + tabBar.height

		GridLayout {
			anchors.fill: parent
			rowSpacing: 0
			columnSpacing: 0
			columns: 2

			ActionButton {
				text: "≣"
				display: AbstractButton.TextOnly
				onClicked: QuickPresenter.toggleDrawer()
			}

			ToolBarLabel {
				text: qsTr("Sample Tabs")
				Layout.fillWidth: true
				Layout.preferredHeight: 56
			}

			TabBar {
				id: tabBar
				Layout.columnSpan: 2
				Layout.fillWidth: true
				onCurrentIndexChanged: swipe.setCurrentIndex(currentIndex)
				Material.background: Material.primary

				TabButton {
					text: "+"
					onClicked: viewModel.addTab();
				}
			}
		}
	}

	PresenterProgress {}

	SwipeView {
		id: swipe
		anchors.fill: parent
		onCurrentIndexChanged: tabBar.setCurrentIndex(currentIndex)
	}

	Component {
		id: _newTab
		TabButton {
			property TabItemViewModel viewModel: null

			text: viewModel.title
		}
	}

	function presentTab(item) {
		tabBar.insertItem(tabBar.count - 1, _newTab.createObject(tabBar, {viewModel: item.viewModel}));
		item.parent = swipe;
		swipe.addItem(item);
		tabBar.setCurrentIndex(tabBar.count - 2);
		return true;
	}

	function afterPop() {
		while(tabBar.count > 0)
			tabBar.takeItem(0).destroy();
		while(swipe.count > 0)
			swipe.takeItem(0).destroy();
	}
}

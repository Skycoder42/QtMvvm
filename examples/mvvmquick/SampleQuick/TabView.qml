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
			currentIndex: 0

			TabButton {
				text: "+"
				onClicked: viewModel.addTab();
			}
		}
	}

	PresenterProgress {}

	SwipeView {
		id: swipe
		anchors.fill: parent
		currentIndex: bar.tabBarItem.currentIndex
	}

	Component {
		id: _newTab
		TabButton {
			property TabItemViewModel viewModel: null

			text: viewModel.title
			onClicked: swipe.setCurrentIndex(bar.tabBarItem.currentIndex)
		}
	}

	function presentTab(item) {
		console.log("here");
		var tabBar = bar.tabBarItem;
		tabBar.insertItem(tabBar.count - 1, _newTab.createObject(tabBar, {viewModel: item.viewModel}));
		item.parent = swipe;
		swipe.addItem(item);
		tabBar.setCurrentIndex(tabBar.count - 2);
		return true;
	}

	function afterPop() {
		var tabBar = bar.tabBarItem;
		while(tabBar.count > 0)
			tabBar.takeItem(0).destroy();
		while(swipe.count > 0)
			swipe.takeItem(0).destroy();
	}
}

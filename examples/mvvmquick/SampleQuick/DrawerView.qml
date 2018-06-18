import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.QtMvvm.Sample 1.1

ListView {
	id: drawerView

	property DrawerViewModel viewModel: null
	property Drawer drawer: null

	model: viewModel.navModel
	anchors.fill: parent
	clip: true

	ScrollBar.vertical: ScrollBar {}

	delegate: ItemDelegate {
		width: parent.width
		text: viewModel.navModel.data(viewModel.navModel.index(index, 0))  //because "display" is not accessible

		onClicked: {
			viewModel.open(index);
			drawer.close();
		}
	}
}

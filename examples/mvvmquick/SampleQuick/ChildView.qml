import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1
import de.skycoder42.QtMvvm.Sample 1.1

Rectangle {
	property ChildViewModel viewModel: null

	color: "red"

	property bool dummyClosed: false

	function closeAction() {
		if(dummyClosed)
			return false;
		else {
			dummyClosed = true;
			label.text = qsTr("I was closed!")
			return true;
		}
	}

	Label {
		id: label
		anchors.centerIn: parent
		text: qsTr("I am the child!")
	}
}

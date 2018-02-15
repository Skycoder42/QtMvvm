import QtQuick 2.10

Rectangle {
	property QtObject viewModel: null

	anchors.fill: parent

	color: "red"

	Component.onCompleted: console.log(viewModel)
}

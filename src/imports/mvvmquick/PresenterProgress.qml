import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.qtmvvm.quick 1.0

ProgressBar {
	visible: QuickPresenter.viewLoading
	value: QuickPresenter.loadingProgress

	anchors.left: parent.left
	anchors.right: parent.right
	anchors.top: parent.top
	z: 10
}

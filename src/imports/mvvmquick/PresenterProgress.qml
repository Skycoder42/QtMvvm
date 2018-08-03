import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.QtMvvm.Quick 1.1

/*! @brief A @ref QtQuick.Controls.ProgressBar "ProgressBar" with automatic bindings to the
 * presenters view loading progress
 *
 * @extends QtQuick.Controls.ProgressBar
 *
 * @details You can use this bar in your views to display the load progress of new views to the
 * user. The bar automatically anchors itself to the top of the view and hides itself when no
 * views are beeing loaded. You can use it as is:
 *
 * @code{.qml}
 * PresenterProgress {}
 * @endcode
 */
ProgressBar {
	visible: QuickPresenter.viewLoading
	value: QuickPresenter.loadingProgress

	anchors.left: parent.left
	anchors.right: parent.right
	anchors.top: parent.top
	z: 10
}

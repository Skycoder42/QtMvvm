#ifndef QTMVVM_QQMLVIEWPLACEHOLDER_H
#define QTMVVM_QQMLVIEWPLACEHOLDER_H

#include <QtQuick/QQuickItem>

#include <QtMvvmCore/ViewModel>

#ifdef DOXYGEN_RUN
namespace de::skycoder42::QtMvvm::Quick {

/*!
 * @brief A placeholder item to show a view within another view
 *
 * @extends QtQuick.Item
 * @since 1.1
 *
 * The following snippet shows how use the placeholder
 * @code{.qml}
Page {
	id: view
	property MyViewModel viewModel

	function presentItem(item) {
		return viewPlaceholder.presentItem(item);
	}

	function closeAction() {
		return viewPlaceholder.closeAction();
	}

	PresenterProgress {}

	ColumnLayout {
		anchors.fill: parent

		ViewPlaceholder {
			id: viewPlaceholder

			Layout.fillWidth: true
			Layout.fillHeight: true

			BusyIndicator {
				anchors.centerIn: parent
				running: !viewPlaceholder.loadedView
			}
		}

		Button {
			Layout.fillWidth: true
			text: viewPlaceholder.loadedView ? qsTr("Discard child") : qsTr("Show Child")
			onClicked: viewPlaceholder.loadedView ? viewPlaceholder.discardView() : viewModel.loadChild();
		}
	}
}
 * @endcode
 */
class ViewPlaceholder
#else
namespace QtMvvm {

class QQmlViewPlaceholder : public QQuickItem
#endif
{
	Q_OBJECT

	/*!
	 * @brief Specify whether to automatically resize the placeholders content view
	 *
	 * @default{`true`}
	 *
	 * When enabled, the view that is loaded into the placeholder is always automatically
	 * resized to exactly fit into the placeholder. If not, the view must resize itself.
	 *
	 * @accessors{
	 *	@memberAc{autoResizeView}
	 *	@notifyAc{autoResizeViewChanged()}
	 * }
	 *
	 * @sa width, height, implicitWidth, implicitHeight
	 */
	Q_PROPERTY(bool autoResizeView MEMBER _autoResizeView NOTIFY autoResizeViewChanged)
	/*!
	 * @brief Specify what happens when showing a new viewmodel while one already exists
	 *
	 * @default{`false`}
	 *
	 * When set to false, trying to present a new viewmodel on this placeholder will fail. If
	 * true, the old one is destroyed and replaced by the new one instead.
	 *
	 * @accessors{
	 *	@memberAc{replaceViews}
	 *	@notifyAc{replaceViewsChanged()}
	 * }
	 */
	Q_PROPERTY(bool replaceViews MEMBER _replaceViews NOTIFY replaceViewsChanged)
	/*!
	 * @brief Specify whether discarding the contained view is a close action
	 *
	 * @default{`false`}
	 *
	 * When the placeholder receives a close action and the contained view does not handle it,
	 * either the placeholder only closes the contained view and thus accepts the action (that
	 * happens if true), or does not accept it keep the view so that the next higher instance
	 * can handle the request (that happens if false).
	 *
	 * @accessors{
	 *	@memberAc{closeViewOnAction}
	 *	@notifyAc{closeViewOnActionChanged()}
	 * }
	 *
	 * @sa ViewPlaceholder::closeAction
	 */
	Q_PROPERTY(bool closeViewOnAction MEMBER _closeViewOnAction NOTIFY closeViewOnActionChanged)

	/*!
	 * @brief Returns the currently loaded view
	 *
	 * @default{`nullptr`}
	 *
	 * If a view is contained in the placeholder, that one is returned. Otherwise nullptr is
	 * returned by the property
	 *
	 * @accessors{
	 *	@memberAc{closeViewOnAction}
	 *	@notifyAc{closeViewOnActionChanged()}
	 *  @readonlyAc
	 * }
	 */
	Q_PROPERTY(QQuickItem* loadedView READ loadedView NOTIFY loadedViewChanged)

public:
	//! @private
	explicit QQmlViewPlaceholder(QQuickItem *parent = nullptr);

	/*!
	 * @brief The presenter method to be called from the parent view
	 *
	 * @param item The item to be presented in this placeholder
	 * @returns true if the item was presented as the placeholders content, false if not
	 *
	 * Implement a method with the same signature in the view that holds the placeholder and
	 * call this method from your implementation. This way you can automatically pass views
	 * to be presented to the placeholder.
	 *
	 * @sa ViewPlaceholder
	 */
	Q_INVOKABLE bool presentItem(QQuickItem *item);
	/*!
	 * @brief A close action that propagates to the contained item
	 *
	 * @returns true if the contained item accepted the action or was discarded, false
	 * otherwise
	 *
	 * Implement a method with the same signature in the view that holds the placeholder and
	 * call this method from your implementation. This way close actions are properly handeled
	 * automatically.
	 *
	 * @sa ViewPlaceholder, ViewPlaceholder::closeViewOnAction
	 */
	Q_INVOKABLE bool closeAction();

	//! @private
	QQuickItem* loadedView() const;

public Q_SLOTS:
	//! Closes the currenty contained view by destoying it
	void discardView();

Q_SIGNALS:
	//! @notifyAcFn{ViewPlaceholder::autoResizeView}
	void autoResizeViewChanged(bool autoResizeView);
	//! @notifyAcFn{ViewPlaceholder::replaceViews}
	void replaceViewsChanged(bool replaceViews);
	//! @notifyAcFn{ViewPlaceholder::closeViewOnAction}
	void closeViewOnActionChanged(bool closeViewOnAction);
	//! @notifyAcFn{ViewPlaceholder::loadedView}
	void loadedViewChanged(QQuickItem* loadedView);

private Q_SLOTS:
	void resizeView();

	void updateImpHeight();
	void updateImpWidth();

private:
	bool _autoResizeView = true;
	bool _replaceViews = false;
	bool _closeViewOnAction = false;

	QPointer<QQuickItem> _loadedView = nullptr;

	bool _isReady = false;
	QMetaObject::Connection _parentVmCon;

	void connectSizeChanges();
	void disconnectSizeChanges(bool resetSize);
};

}

#endif // QTMVVM_QQMLVIEWPLACEHOLDER_H

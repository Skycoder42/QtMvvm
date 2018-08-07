#ifndef QTMVVM_QQMLQUICKPRESENTER_H
#define QTMVVM_QQMLQUICKPRESENTER_H

#include <tuple>

#include <QtCore/QObject>
#include <QtCore/QCache>
#include <QtCore/QVariant>
#include <QtCore/QPointer>
#include <QtCore/QUrl>
#include <QtCore/QQueue>
#include <QtCore/QSharedPointer>

#include <QtGui/QColor>

#include <QtQml/QQmlComponent>

#include <QtMvvmCore/ViewModel>
#include <QtMvvmCore/Messages>

#include <QtMvvmQuick/InputViewFactory>

#ifdef DOXYGEN_RUN
namespace de::skycoder42::QtMvvm::Quick {

/*! @brief A QML singleton to access common presenter methods globally
 *
 * @extends QtQml.QtObject
 * @since 1.0
 *
 * The main purpose of the class is to create a communication channel between the QML code and
 * the C++ presenter logic.
 *
 * @sa QtMvvm::QuickPresenter
 */
class QuickPresenter : public QtObject
#else
namespace QtMvvm {

class QQmlQuickPresenter : public QObject
#endif
{
	Q_OBJECT

	/*! @brief The name of the currently active Quick Controls 2 Style
	 *
	 * @default{`QQuickStyle::name()`}
	 *
	 * @accessors{
	 *	@memberAc{currentStyle}
	 *  @readonlyAc
	 *  @constantAc
	 * }
	 */
	Q_PROPERTY(QString currentStyle READ currentStyle CONSTANT)
	/*! @brief The view factory provided by the C++ presenter
	 *
	 * @default{`QtMvvm::QuickPresenter::inputViewFactory`}
	 *
	 * @accessors{
	 *	@memberAc{inputViewFactory}
	 *  @notifyAc{inputViewFactoryChanged()}
	 *  @readonlyAc
	 * }
	 */
	Q_PROPERTY(InputViewFactory* inputViewFactory READ inputViewFactory NOTIFY inputViewFactoryChanged)

	/*! @brief The primary presenter QML object.
	 *
	 * @default{`null`}
	 *
	 * This property must be set in the root QML component that is loaded by the qml
	 * application in order to actually make presenting possible. When using the QtMvvmApp,
	 * this is take care of automatically.
	 *
	 * @accessors{
	 *	@memberAc{qmlPresenter}
	 *  @notifyAc{qmlPresenterChanged()}
	 * }
	 *
	 * @sa QtMvvmApp
	 */
	Q_PROPERTY(QObject* qmlPresenter MEMBER _qmlPresenter NOTIFY qmlPresenterChanged)
	/*! @brief Specifies whether the presenter is currently loading a new view component
	 *
	 * @default{`false`}
	 *
	 * @accessors{
	 *	@memberAc{viewLoading}
	 *  @notifyAc{viewLoadingChanged()}
	 *  @readonlyAc
	 * }
	 *
	 * @sa QuickPresenter::loadingProgress
	 */
	Q_PROPERTY(bool viewLoading READ isViewLoading NOTIFY viewLoadingChanged)
	/*! @brief Reports the current loading progress when currently loading a new view
	 *
	 * @default{`0.0`}
	 *
	 * Is limited to the interval `[0.0,0.1]`
	 *
	 * @accessors{
	 *	@memberAc{loadingProgress}
	 *  @notifyAc{loadingProgressChanged()}
	 *  @readonlyAc
	 * }
	 *
	 * @sa QuickPresenter::viewLoading
	 */
	Q_PROPERTY(qreal loadingProgress READ loadingProgress NOTIFY loadingProgressChanged)

public:
	//! @private
	explicit QQmlQuickPresenter(QQmlEngine *engine);

	//! @private
	QString currentStyle() const;
	//! @private
	InputViewFactory* inputViewFactory() const;
	//! @private
	bool isViewLoading() const;
	//! @private
	qreal loadingProgress() const;

#ifndef DOXYGEN_RUN
#define static
#endif
	//! Converts a list of mimetypes into a list of extension filters, for a file dialog
	Q_INVOKABLE static QStringList mimeTypeFilters(const QStringList &mimeTypes) const;

	/*! @brief Calculates the optimal text color based on the background color
	 *
	 * @param accentColor The color to find a contrasting text color for
	 * @param baseColor The current default text color
	 * @return A color for text that is easy to read when used with accentColor as
	 * the background
	 *
	 * The method calculates whether the color should be light or dark and then either returns
	 * white or black, depending on which color fits better. If baseColor is specified, then
	 * the color value is checked, too. If baseColor is easily readable, it is simply returned
	 * as result. Otherwise the method proceeds as usual.
	 */
	QTMVVM_REVISION_1 Q_INVOKABLE static QColor accentTextColor(const QColor &accentColor, const QColor &baseColor) const;

#ifdef DOXYGEN_RUN
public:
#else
public Q_SLOTS:
#endif
	//! Toggles the state of the current drawer, if any is beeing used
	static void toggleDrawer();
	//! Pops the current top level view
	static void popView();

	//! Performs haptic feedback of a long press (Android only)
	static void hapticLongPress();

#ifndef DOXYGEN_RUN
#undef static
#endif

Q_SIGNALS:
	//! @notifyAcFn{QuickPresenter::qmlPresenter}
	void qmlPresenterChanged(QObject* qmlPresenter);
	//! @notifyAcFn{QuickPresenter::viewLoading}
	void viewLoadingChanged(bool viewLoading);
	//! @notifyAcFn{QuickPresenter::loadingProgress}
	void loadingProgressChanged(qreal loadingProgress);
	//! @notifyAcFn{QuickPresenter::inputViewFactory}
	void inputViewFactoryChanged(InputViewFactory* inputViewFactory);

private Q_SLOTS:
	void present(QtMvvm::ViewModel *viewModel, const QVariantHash &params, const QUrl &viewUrl, QPointer<QtMvvm::ViewModel> parent);
	void showDialog(const QtMvvm::MessageConfig &config, QtMvvm::MessageResult *result);
	void statusChanged(QQmlComponent::Status status);

private:
	using PresentTuple = std::tuple<QSharedPointer<QQmlComponent>, ViewModel*, QVariantHash, QPointer<ViewModel>>;
	QQmlEngine *_engine;
	QPointer<QObject> _qmlPresenter;

	QPointer<QQmlComponent> _latestComponent;
	QCache<QUrl, QSharedPointer<QQmlComponent>> _componentCache;
	QQueue<PresentTuple> _loadQueue;

	void processShowQueue();
	void addObject(QQmlComponent *component, ViewModel *viewModel, const QVariantHash &params, const QPointer<ViewModel> &parent);
};

}

#endif // QTMVVM_QQMLQUICKPRESENTER_H

#ifndef QTMVVM_QQMLQUICKPRESENTER_H
#define QTMVVM_QQMLQUICKPRESENTER_H

#include <tuple>

#include <QtCore/QObject>
#include <QtCore/QCache>
#include <QtCore/QVariant>
#include <QtCore/QPointer>
#include <QtCore/QUrl>

#include <QtQml/QQmlComponent>

#include <QtMvvmCore/ViewModel>
#include <QtMvvmCore/Messages>

namespace QtMvvm {

class QQmlQuickPresenter : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString currentStyle READ currentStyle CONSTANT)

	Q_PROPERTY(QObject* qmlPresenter MEMBER _qmlPresenter NOTIFY qmlPresenterChanged)
	Q_PROPERTY(bool viewLoading READ isViewLoading NOTIFY viewLoadingChanged)
	Q_PROPERTY(qreal loadingProgress READ loadingProgress NOTIFY loadingProgressChanged)

public:
	explicit QQmlQuickPresenter(QQmlEngine *engine);

	QString currentStyle() const;

	bool isViewLoading() const;
	qreal loadingProgress() const;

Q_SIGNALS:
	void qmlPresenterChanged(QObject* qmlPresenter);
	void viewLoadingChanged(bool viewLoading);
	void loadingProgressChanged(qreal loadingProgress);

private Q_SLOTS:
	void present(QtMvvm::ViewModel *viewModel, const QVariantHash &params, const QUrl &viewUrl, QPointer<QtMvvm::ViewModel> parent);
	void showDialog(const QtMvvm::MessageConfig &config, QtMvvm::MessageResult *result);
	void statusChanged(QQmlComponent::Status status);

private:
	typedef std::tuple<ViewModel*, QVariantHash, QPointer<ViewModel>> PresentTuple;
	QQmlEngine *_engine;
	QPointer<QObject> _qmlPresenter;

	QPointer<QQmlComponent> _latestComponent;
	QCache<QUrl, QQmlComponent> _componentCache;
	QHash<QQmlComponent*, PresentTuple> _loadCache;

	void addObject(QQmlComponent *component, ViewModel *viewModel, const QVariantHash &params, QPointer<ViewModel> parent);
};

}

#endif // QTMVVM_QQMLQUICKPRESENTER_H

#ifndef QTMVVM_QQMLVIEWPLACEHOLDER_H
#define QTMVVM_QQMLVIEWPLACEHOLDER_H

#include <QtQuick/QQuickItem>

#include <QtMvvmCore/ViewModel>

namespace QtMvvm {

class QQmlViewPlaceholder : public QQuickItem
{
	Q_OBJECT

	Q_PROPERTY(QString viewModelType MEMBER _viewModelType NOTIFY viewModelTypeChanged)
	Q_PROPERTY(QVariantHash showParams MEMBER _showParams NOTIFY showParamsChanged)
	Q_PROPERTY(QtMvvm::ViewModel* parentViewModel MEMBER _parentViewModel WRITE setParentViewModel NOTIFY parentViewModelChanged)
	Q_PROPERTY(bool autoPresent MEMBER _autoPresent NOTIFY autoPresentChanged)
	Q_PROPERTY(bool autoResizeView MEMBER _autoResizeView NOTIFY autoResizeViewChanged)
	Q_PROPERTY(bool replaceViews MEMBER _replaceViews NOTIFY replaceViewsChanged)
	Q_PROPERTY(bool closeViewOnAction MEMBER _closeViewOnAction NOTIFY closeViewOnActionChanged)

	Q_PROPERTY(QQuickItem* loadedView READ loadedView NOTIFY loadedViewChanged)

public:
	explicit QQmlViewPlaceholder(QQuickItem *parent = nullptr);

	Q_INVOKABLE bool presentItem(const QVariant &item);
	Q_INVOKABLE bool closeAction();

	void setParentViewModel(ViewModel *parentViewModel);
	QQuickItem* loadedView() const;

	void componentComplete() override;

public Q_SLOTS:
	void presentView();
	void discardView();

Q_SIGNALS:
	void viewModelTypeChanged(const QString &viewModelType);
	void showParamsChanged(const QVariantHash &showParams);
	void parentViewModelChanged(QtMvvm::ViewModel* parentViewModel);
	void autoPresentChanged(bool autoPresent);
	void autoResizeViewChanged(bool autoResizeView);
	void replaceViewsChanged(bool replaceViews);
	void closeViewOnActionChanged(bool closeViewOnAction);
	void loadedViewChanged(QQuickItem* loadedView);

private Q_SLOTS:
	void parentItemVmChanged(QtMvvm::ViewModel *viewModel);
	void presentIfReady();

	void resizeView();

private:
	QString _viewModelType;
	QVariantHash _showParams;
	ViewModel *_parentViewModel = nullptr;
	bool _autoPresent = true;
	bool _autoResizeView = true;
	bool _replaceViews = false;
	bool _closeViewOnAction = false;

	QPointer<QQuickItem> _loadedView = nullptr;

	bool _isReady = false;
	bool _clearParentVmCon = true;
	QMetaObject::Connection _parentVmCon;

	void getParentViewModel();
	void connectSizeChanges();
	void disconnectSizeChanges();
};

}

#endif // QTMVVM_QQMLVIEWPLACEHOLDER_H

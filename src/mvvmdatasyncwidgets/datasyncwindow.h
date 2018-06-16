#ifndef QTMVVM_DATASYNCWINDOW_H
#define QTMVVM_DATASYNCWINDOW_H

#include <QtCore/QScopedPointer>

#include <QtWidgets/QWidget>

#include <QtMvvmDataSyncCore/DataSyncViewModel>

#include "QtMvvmDataSyncWidgets/qtmvvmdatasyncwidgets_global.h"

namespace QtMvvm {

class DataSyncWindowPrivate;
//! The widgets view implementation for the DataSyncViewModel
class Q_MVVMDATASYNCWIDGETS_EXPORT DataSyncWindow : public QWidget
{
	Q_OBJECT

	//! A helper property to set and reset the sync progress automatically
	Q_PROPERTY(double syncProgress READ syncProgress WRITE setSyncProgress)
	//! A helper property to set the error text properly
	Q_PROPERTY(QString errorText READ errorText WRITE setErrorText)

public:
	//! View constructor
	Q_INVOKABLE explicit DataSyncWindow(QtMvvm::ViewModel *viewModel, QWidget *parent = nullptr);
	~DataSyncWindow() override;

	//! @readAcFn{DataSyncWindow::syncProgress}
	double syncProgress() const;
	//! @readAcFn{DataSyncWindow::errorText}
	QString errorText() const;

public Q_SLOTS:
	//! @writeAcFn{DataSyncWindow::syncProgress}
	void setSyncProgress(double syncProgress);
	//! @writeAcFn{DataSyncWindow::errorText}
	void setErrorText(QString errorText);

protected Q_SLOTS:
	//! Is called by the view as soon as the viewmodel has been completly loaded
	virtual void viewModelReady();

private Q_SLOTS:
	void removeCurrentDevice();

private:
	QScopedPointer<DataSyncWindowPrivate> d;
};

}

#endif // QTMVVM_DATASYNCWINDOW_H

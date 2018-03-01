#ifndef QTMVVM_DATASYNCWINDOW_H
#define QTMVVM_DATASYNCWINDOW_H

#include <QtCore/QScopedPointer>

#include <QtWidgets/QWidget>

#include <QtMvvmDataSyncCore/DataSyncViewModel>

#include "QtMvvmDataSyncWidgets/qtmvvmdatasyncwidgets_global.h"

namespace QtMvvm {

class DataSyncWindowPrivate;
class Q_MVVMDATASYNCWIDGETS_EXPORT DataSyncWindow : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(double syncProgress READ syncProgress WRITE setSyncProgress)
	Q_PROPERTY(QString errorText READ errorText WRITE setErrorText)

public:
	Q_INVOKABLE explicit DataSyncWindow(QtMvvm::ViewModel *viewModel, QWidget *parent = nullptr);
	~DataSyncWindow();

	double syncProgress() const;
	QString errorText() const;

public Q_SLOTS:
	void setSyncProgress(double syncProgress);
	void setErrorText(QString errorText);

protected Q_SLOTS:
	virtual void viewModelReady();

private Q_SLOTS:
	void removeCurrentDevice();

private:
	QScopedPointer<DataSyncWindowPrivate> d;
};

}

#endif // QTMVVM_DATASYNCWINDOW_H

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

public:
	Q_INVOKABLE explicit DataSyncWindow(QtMvvm::ViewModel *viewModel, QWidget *parent = nullptr);
	~DataSyncWindow();

private:
	QScopedPointer<DataSyncWindowPrivate> d;
};

}

#endif // QTMVVM_DATASYNCWINDOW_H

#ifndef QTMVVM_NETWORKEXCHANGEWINDOW_H
#define QTMVVM_NETWORKEXCHANGEWINDOW_H

#include <QtCore/qscopedpointer.h>

#include <QtMvvmDataSyncCore/networkexchangeviewmodel.h>

#include <QtWidgets/qwidget.h>

#include "QtMvvmDataSyncWidgets/qtmvvmdatasyncwidgets_global.h"

namespace QtMvvm {

class NetworkExchangeWindowPrivate;
class Q_MVVMDATASYNCWIDGETS_EXPORT NetworkExchangeWindow : public QWidget
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit NetworkExchangeWindow(QtMvvm::ViewModel *viewModel, QWidget *parent = nullptr);
	~NetworkExchangeWindow();

private Q_SLOTS:
	void activated(const QModelIndex &index);

private:
	QScopedPointer<NetworkExchangeWindowPrivate> d;
};

}

#endif // QTMVVM_NETWORKEXCHANGEWINDOW_H

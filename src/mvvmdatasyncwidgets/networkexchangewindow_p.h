#ifndef QTMVVM_NETWORKEXCHANGEWINDOW_P_H
#define QTMVVM_NETWORKEXCHANGEWINDOW_P_H

#include "qtmvvmdatasyncwidgets_global.h"
#include "networkexchangewindow.h"

namespace Ui {
class NetworkExchangeWindow;
}

namespace QtMvvm {

class NetworkExchangeWindowPrivate
{
	Q_DISABLE_COPY(NetworkExchangeWindowPrivate)

public:
	NetworkExchangeWindowPrivate(ViewModel *viewModel);

	NetworkExchangeViewModel *viewModel;
	QScopedPointer<Ui::NetworkExchangeWindow> ui;
};

}

#endif // QTMVVM_NETWORKEXCHANGEWINDOW_P_H

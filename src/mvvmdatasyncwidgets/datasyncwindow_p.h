#ifndef QTMVVM_DATASYNCWINDOW_P_H
#define QTMVVM_DATASYNCWINDOW_P_H

#include "qtmvvmdatasyncwidgets_global.h"
#include "datasyncwindow.h"

namespace Ui {
class DataSyncWindow;
}

namespace QtMvvm {

class DataSyncWindowPrivate
{
	Q_DISABLE_COPY(DataSyncWindowPrivate)

public:
	DataSyncWindowPrivate(ViewModel *viewModel);

	DataSyncViewModel *viewModel;
	QScopedPointer<Ui::DataSyncWindow> ui;
};

}

#endif // QTMVVM_DATASYNCWINDOW_P_H

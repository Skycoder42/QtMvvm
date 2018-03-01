#include "qtmvvmdatasyncwidgets_global.h"
#include <QtMvvmWidgets/WidgetsPresenter>

#include "datasyncwindow.h"

void QtMvvm::registerDataSyncWidgets()
{
	QtMvvm::WidgetsPresenter::registerView<DataSyncWindow>();
}

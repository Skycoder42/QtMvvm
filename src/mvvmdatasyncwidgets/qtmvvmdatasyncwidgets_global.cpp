#include "qtmvvmdatasyncwidgets_global.h"
#include <QtMvvmWidgets/WidgetsPresenter>

#include "datasyncwindow.h"
#include "exportsetupdialog_p.h"

void QtMvvm::registerDataSyncWidgets()
{
	QtMvvm::WidgetsPresenter::registerView<DataSyncWindow>();
	QtMvvm::WidgetsPresenter::registerView<ExportSetupDialog>();
}

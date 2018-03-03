#include "qtmvvmdatasyncwidgets_global.h"
#include <QtMvvmWidgets/WidgetsPresenter>

#include "datasyncwindow.h"
#include "networkexchangewindow.h"
#include "exportsetupdialog_p.h"
#include "changeremotedialog_p.h"
#include "identityeditdialog_p.h"

void QtMvvm::registerDataSyncWidgets()
{
	QtMvvm::WidgetsPresenter::registerView<DataSyncWindow>();
	QtMvvm::WidgetsPresenter::registerView<NetworkExchangeWindow>();
	QtMvvm::WidgetsPresenter::registerView<ExportSetupDialog>();
	QtMvvm::WidgetsPresenter::registerView<ChangeRemoteDialog>();
	QtMvvm::WidgetsPresenter::registerView<IdentityEditDialog>();
}

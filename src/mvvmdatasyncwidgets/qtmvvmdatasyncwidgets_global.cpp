#include "qtmvvmdatasyncwidgets_global.h"
#include <QtMvvmWidgets/WidgetsPresenter>

#include "datasyncwindow.h"
#include "networkexchangewindow.h"
#include "exportsetupdialog_p.h"
#include "changeremotedialog_p.h"
#include "identityeditdialog_p.h"

static void initResources()
{
#ifdef QT_STATIC
	Q_INIT_RESOURCE(qtmvvmdatasyncwidgets_module);
#endif
}

void QtMvvm::registerDataSyncWidgets()
{
	initResources();
	QtMvvm::WidgetsPresenter::registerView<DataSyncWindow>();
	QtMvvm::WidgetsPresenter::registerView<NetworkExchangeWindow>();
	QtMvvm::WidgetsPresenter::registerView<ExportSetupDialog>();
	QtMvvm::WidgetsPresenter::registerView<ChangeRemoteDialog>();
	QtMvvm::WidgetsPresenter::registerView<IdentityEditDialog>();
}

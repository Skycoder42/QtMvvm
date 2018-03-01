#include "datasyncwindow.h"
#include "datasyncwindow_p.h"
#include "ui_datasyncwindow.h"
using namespace QtMvvm;

DataSyncWindow::DataSyncWindow(QtMvvm::ViewModel *viewModel, QWidget *parent) :
	QWidget(parent, Qt::Window),
	d(new DataSyncWindowPrivate(this, viewModel))
{
	d->ui->setupUi(this);
}

DataSyncWindow::~DataSyncWindow() {}

// ------------- Private Implementation -------------

DataSyncWindowPrivate::DataSyncWindowPrivate(DataSyncWindow *q_ptr, ViewModel *viewModel) :
	viewModel(static_cast<DataSyncViewModel*>(viewModel)),
	ui(new Ui::DataSyncWindow())
{}

#include "datasyncwindow.h"
#include "datasyncwindow_p.h"
#include "ui_datasyncwindow.h"

#include <QtMvvmCore/Binding>

#include <QtWidgets/QMenu>

using namespace QtMvvm;
using namespace QtDataSync;

DataSyncWindow::DataSyncWindow(QtMvvm::ViewModel *viewModel, QWidget *parent) :
	QWidget(parent, Qt::Window),
	d(new DataSyncWindowPrivate(viewModel))
{
	d->ui->setupUi(this);

	//ui setup
	d->ui->identityButton->setDefaultAction(d->ui->actionEdit_Identity);

	d->ui->treeView->addActions({
									d->ui->actionRe_load_Device_List,
									d->ui->action_Remove_Device
								});

	auto accountMenu = new QMenu(d->ui->accountButton);
	accountMenu->addAction(d->ui->action_Network_exchange);
	accountMenu->addAction(d->ui->action_Export_to_file);
	accountMenu->addAction(d->ui->action_Import_from_file);
	accountMenu->addSeparator();
	accountMenu->addAction(d->ui->actionRe_load_Device_List);
	accountMenu->addAction(d->ui->action_Remove_Device);
	accountMenu->addSeparator();
	accountMenu->addAction(d->ui->actionUpdate_Exchange_Key);
	accountMenu->addSeparator();
	accountMenu->addAction(d->ui->action_Change_Remote_Server);
	accountMenu->addAction(d->ui->action_Reset_Identity);
	d->ui->accountButton->setMenu(accountMenu);

	//viewmodel stuff
	connect(d->viewModel, &DataSyncViewModel::ready,
			this, &DataSyncWindow::viewModelReady);
}

DataSyncWindow::~DataSyncWindow() = default;

double DataSyncWindow::syncProgress() const
{
	return d->ui->progressBar->value() / 1000.0;
}

QString DataSyncWindow::errorText() const
{
	return d->ui->errorLabel->text();
}

void DataSyncWindow::setSyncProgress(double syncProgress)
{
	if(syncProgress < 0)
		d->ui->progressBar->reset();
	else
		d->ui->progressBar->setValue(static_cast<int>(syncProgress * 1000));
}

void DataSyncWindow::setErrorText(QString errorText)
{
	d->ui->errorLabel->setText(errorText);
	d->ui->stackedWidget->setCurrentIndex(errorText.isEmpty() ? 0 : 1);
}

void DataSyncWindow::viewModelReady()
{
	//sync manager bindings
	bind(d->viewModel->syncManager(), "syncEnabled",
		 d->ui->syncCheckBox, "checked");
	bind(d->viewModel, "statusString",
		 d->ui->statusLabel, "text",
		 Binding::OneWayToView);
	bind(d->viewModel->syncManager(), "syncProgress",
		 this, "syncProgress",
		 Binding::OneWayToView);
	bind(d->viewModel->syncManager(), "lastError",
		 this, "errorText",
		 Binding::OneWayToView);
	//account manager bindings
	d->ui->treeView->setModel(d->viewModel->sortedModel());

	//sync manager connections
	connect(d->ui->syncButton, &QCommandLinkButton::clicked,
			d->viewModel, &DataSyncViewModel::syncOrConnect);
	//account manager connections
	connect(d->ui->actionEdit_Identity, &QAction::triggered,
			d->viewModel, &DataSyncViewModel::showDeviceInfo);
	connect(d->ui->action_Export_to_file, &QAction::triggered,
			d->viewModel, &DataSyncViewModel::startExport);
	connect(d->ui->action_Import_from_file, &QAction::triggered,
			d->viewModel, &DataSyncViewModel::startImport);
	connect(d->ui->action_Reset_Identity, &QAction::triggered,
			d->viewModel, &DataSyncViewModel::performReset);
	connect(d->ui->action_Change_Remote_Server, &QAction::triggered,
			d->viewModel, &DataSyncViewModel::changeRemote);
	connect(d->ui->action_Network_exchange, &QAction::triggered,
			d->viewModel, &DataSyncViewModel::startNetworkExchange);
	connect(d->ui->actionUpdate_Exchange_Key, &QAction::triggered,
			d->viewModel->accountManager(), &AccountManager::updateExchangeKey);
	connect(d->ui->actionRe_load_Device_List, &QAction::triggered,
			d->viewModel->accountManager(), &AccountManager::listDevices);
	connect(d->ui->action_Remove_Device, &QAction::triggered,
			this, &DataSyncWindow::removeCurrentDevice);
	connect(d->ui->actionUpdate_Exchange_Key, &QAction::triggered,
			d->ui->actionUpdate_Exchange_Key, [this](){
		d->ui->actionUpdate_Exchange_Key->setEnabled(false);
	});

	//update header
	d->ui->treeView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
}

void DataSyncWindow::removeCurrentDevice()
{
	auto index = d->ui->treeView->currentIndex();
	if(index.isValid())
		d->viewModel->removeDevice(index.row());
}

// ------------- Private Implementation -------------

DataSyncWindowPrivate::DataSyncWindowPrivate(ViewModel *viewModel) :
	viewModel(static_cast<DataSyncViewModel*>(viewModel)),
	ui(new Ui::DataSyncWindow())
{}

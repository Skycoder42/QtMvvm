#include "networkexchangewindow.h"
#include "networkexchangewindow_p.h"
#include "ui_networkexchangewindow.h"
#include <QtMvvmCore/Binding>
using namespace QtMvvm;

NetworkExchangeWindow::NetworkExchangeWindow(ViewModel *viewModel, QWidget *parent) :
	QWidget(parent, Qt::Window),
	d(new NetworkExchangeWindowPrivate(viewModel))
{
	d->ui->setupUi(this);

	connect(d->ui->treeView, &QTreeView::activated,
			this, &NetworkExchangeWindow::activated);

	bind(d->viewModel, "port",
		 d->ui->exchangePortSpinBox, "value");
	bind(d->viewModel, "deviceName",
		 d->ui->deviceNameLineEdit, "text",
		 Binding::TwoWay,
		 nullptr, "editingFinished()");
	bind(d->viewModel, "active",
		 d->ui->exchangeCheckBox, "checked");

	d->ui->treeView->setModel(d->viewModel->sortedModel());
}

NetworkExchangeWindow::~NetworkExchangeWindow() = default;

void NetworkExchangeWindow::activated(const QModelIndex &index)
{
	if(index.isValid())
		d->viewModel->exportTo(index.row());
}

// ------------- Private Implementation -------------

NetworkExchangeWindowPrivate::NetworkExchangeWindowPrivate(ViewModel *viewModel) :
	viewModel(static_cast<NetworkExchangeViewModel*>(viewModel)),
	ui(new Ui::NetworkExchangeWindow())
{}

#include "changeremotedialog_p.h"
#include "ui_changeremotedialog.h"
#include <QtMvvmCore/Binding>
#include <QtMvvmWidgets/private/widgetspresenter_p.h>
#include <QtWidgets/QPushButton>
using namespace QtMvvm;

ChangeRemoteDialog::ChangeRemoteDialog(ViewModel *viewModel, QWidget *parent) :
	QDialog(parent),
	_viewModel(static_cast<ChangeRemoteViewModel*>(viewModel)),
	ui(new Ui::ChangeRemoteDialog)
{
	ui->setupUi(this);
	if(parentWidget()) {
		setWindowModality(Qt::WindowModal);
		setWindowFlags(Qt::Sheet | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
	} else {
		setWindowModality(Qt::ApplicationModal);
		setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
	}

	ui->addHeaderButton->setDefaultAction(ui->actionA_dd_Header);
	ui->removeHeaderButton->setDefaultAction(ui->action_Remove_Header);
	ui->treeView->addActions({
								 ui->actionA_dd_Header,
								 ui->action_Remove_Header
							 });
	ui->urlLineEdit->setValidator(createUrlValidator({QStringLiteral("ws"), QStringLiteral("wss")}, ui->urlLineEdit));

	bind(_viewModel, "url",
		 ui->urlLineEdit, "text",
		 Binding::OneWayToViewModel);
	bind(_viewModel, "accessKey",
		 ui->accessKeyLineEdit, "text",
		 Binding::OneWayToViewModel);
	bind(_viewModel, "keepAlive",
		 ui->keepAliveTimeoutSpinBox, "value");
	bind(_viewModel, "keepData",
		 ui->keepDataCheckBox, "checked");
	bind(_viewModel, "valid",
		 ui->buttonBox->button(QDialogButtonBox::Ok), "enabled",
		 Binding::OneWayToView);

	connect(ui->actionA_dd_Header, &QAction::triggered,
			this, &ChangeRemoteDialog::addHeader);
	connect(ui->action_Remove_Header, &QAction::triggered,
			this, &ChangeRemoteDialog::removeHeader);

	ui->treeView->setModel(_viewModel->headerModel());
	ui->treeView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
}

ChangeRemoteDialog::~ChangeRemoteDialog() = default;

void ChangeRemoteDialog::accept()
{
	if(_viewModel->completeSetup())
		QDialog::accept();
}

void ChangeRemoteDialog::addHeader()
{
	if(ui->keyEdit->text().isEmpty())
		return;
	_viewModel->addHeaderConfig(ui->keyEdit->text(), ui->valueEdit->text());
	ui->keyEdit->clear();
	ui->valueEdit->clear();
}

void ChangeRemoteDialog::removeHeader()
{
	auto index = ui->treeView->currentIndex();
	if(index.isValid())
		_viewModel->removeHeaderConfig(index.row());
}

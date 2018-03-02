#include "exportsetupdialog_p.h"
#include "ui_exportsetupdialog.h"
#include <QtMvvmCore/Binding>
using namespace QtMvvm;

ExportSetupDialog::ExportSetupDialog(ViewModel *viewModel, QWidget *parent) :
	QDialog(parent),
	_viewModel(static_cast<ExportSetupViewModel*>(viewModel)),
	ui(new Ui::ExportSetupDialog)
{
	ui->setupUi(this);

	bind(_viewModel, "trusted",
		 ui->trustedCheckBox, "checked");
	bind(_viewModel, "includeServer",
		 ui->includeServerCheckBox, "checked");
	bind(_viewModel, "password",
		 ui->passwordLineEdit, "text");
	bind(_viewModel, "valid",
		 ui->buttonBox->button(QDialogButtonBox::Ok), "enabled",
		 Binding::OneWayToView);
}

ExportSetupDialog::~ExportSetupDialog() {}

void ExportSetupDialog::accept()
{
	if(_viewModel->completeSetup())
		QDialog::accept();
}

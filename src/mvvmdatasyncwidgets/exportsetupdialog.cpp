#include "exportsetupdialog_p.h"
#include "ui_exportsetupdialog.h"
#include <QtMvvmCore/Binding>
#include <QtWidgets/QPushButton>
using namespace QtMvvm;

ExportSetupDialog::ExportSetupDialog(ViewModel *viewModel, QWidget *parent) :
	QDialog(parent),
	_viewModel(static_cast<ExportSetupViewModel*>(viewModel)),
	ui(new Ui::ExportSetupDialog)
{
	ui->setupUi(this);
	if(parentWidget()) {
		setWindowModality(Qt::WindowModal);
		setWindowFlags(Qt::Sheet | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
	} else {
		setWindowModality(Qt::ApplicationModal);
		setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
	}

	bind(_viewModel, "label",
		 ui->titleLabel, "text",
		 Binding::OneWayToView);
	bind(_viewModel, "trusted",
		 ui->trustedCheckBox, "checked");
	bind(_viewModel, "includeServer",
		 ui->includeServerCheckBox, "checked");
	bind(_viewModel, "password",
		 ui->passwordLineEdit, "text",
		 Binding::OneWayToViewModel);
	bind(_viewModel, "valid",
		 ui->buttonBox->button(QDialogButtonBox::Ok), "enabled",
		 Binding::OneWayToView);
}

ExportSetupDialog::~ExportSetupDialog() = default;

void ExportSetupDialog::accept()
{
	if(_viewModel->completeSetup())
		QDialog::accept();
}

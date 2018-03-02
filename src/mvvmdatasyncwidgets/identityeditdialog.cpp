#include "identityeditdialog_p.h"
#include "ui_identityeditdialog.h"
#include <QtMvvmCore/Binding>
using namespace QtMvvm;

IdentityEditDialog::IdentityEditDialog(ViewModel *viewModel, QWidget *parent) :
	QDialog(parent),
	_viewModel(static_cast<IdentityEditViewModel*>(viewModel)),
	ui(new Ui::IdentityEditDialog)
{
	ui->setupUi(this);
	if(parentWidget()) {
		setWindowModality(Qt::WindowModal);
		setWindowFlags(Qt::Sheet | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
	} else {
		setWindowModality(Qt::ApplicationModal);
		setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
	}

	bind(_viewModel, "name",
		 ui->deviceNameLineEdit, "text",
		 Binding::TwoWay,
		 nullptr, "editingFinished()");
	bind(_viewModel, "fingerPrint",
		 ui->fingerPrintDisplayLabel, "text",
		 Binding::OneWayToView);
}

IdentityEditDialog::~IdentityEditDialog() {}

void IdentityEditDialog::accept()
{
	_viewModel->save();
	QDialog::accept();
}

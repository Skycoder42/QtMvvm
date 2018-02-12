#include "resultdialog.h"
#include "ui_resultdialog.h"
#include <QtMvvmCore/Binding>

ResultDialog::ResultDialog(QtMvvm::ViewModel *viewModel, QWidget *parent) :
	QDialog(parent),
	_viewModel(static_cast<ResultViewModel*>(viewModel)),
	ui(new Ui::ResultDialog)
{
	ui->setupUi(this);

	QtMvvm::bind(_viewModel, "result",
				 ui->lineEdit, "text",
				 QtMvvm::Binding::TwoWay,
				 nullptr,
				 "editingFinished()");
	connect(this, &ResultDialog::accepted,
			_viewModel, &ResultViewModel::done);
}

ResultDialog::~ResultDialog()
{
	delete ui;
}

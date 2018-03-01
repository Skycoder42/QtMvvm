#include "samplewindow.h"
#include "ui_samplewindow.h"

SampleWindow::SampleWindow(QtMvvm::ViewModel *viewModel, QWidget *parent) :
	QMainWindow(parent),
	_viewModel(static_cast<SampleViewModel*>(viewModel)),
	ui(new Ui::SampleWindow)
{
	ui->setupUi(this);

	ui->listView->setModel(_viewModel->model());
	connect(ui->syncButton, &QPushButton::clicked,
			_viewModel, &SampleViewModel::showSyncInfo);
}

SampleWindow::~SampleWindow()
{
	delete ui;
}

void SampleWindow::on_addButton_clicked()
{
	_viewModel->addData(ui->keyEdit->text());
}

void SampleWindow::on_removeButton_clicked()
{
	_viewModel->removeAt(ui->listView->currentIndex().row());
}

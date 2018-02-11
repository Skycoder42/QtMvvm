#include "sampleview.h"
#include "ui_sampleview.h"

SampleView::SampleView(QtMvvm::ViewModel *viewModel, QWidget *parent) :
	QMainWindow(parent),
	_viewModel(static_cast<SampleViewModel*>(viewModel)),
	ui(new Ui::SampleView)
{
	Q_ASSERT(_viewModel);
	ui->setupUi(this);
}

SampleView::~SampleView()
{
	delete ui;
}

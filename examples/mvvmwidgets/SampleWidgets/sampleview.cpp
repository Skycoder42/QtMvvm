#include "sampleview.h"
#include "ui_sampleview.h"
#include <QtMvvmCore/Binding>

SampleView::SampleView(QtMvvm::ViewModel *viewModel, QWidget *parent) :
	QMainWindow(parent),
	_viewModel(static_cast<SampleViewModel*>(viewModel)),
	ui(new Ui::SampleView)
{
	Q_ASSERT(_viewModel);
	ui->setupUi(this);

	QtMvvm::bind(_viewModel, "active",
				 ui->activeCheckBox, "checked");
	QtMvvm::bind(_viewModel, "name",
				 ui->nameLineEdit, "text",
				 QtMvvm::Binding::TwoWay,
				 nullptr,
				 "editingFinished()");
	ui->eventsListView->setModel(_viewModel->eventsModel());
	connect(ui->clearButton, &QPushButton::clicked,
			_viewModel, &SampleViewModel::clearEvents);
}

SampleView::~SampleView()
{
	delete ui;
}

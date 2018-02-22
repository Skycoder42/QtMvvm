#include "tabitemview.h"
#include "ui_tabitemview.h"
#include <QtMvvmCore/Binding>

TabItemView::TabItemView(QtMvvm::ViewModel *viewModel, QWidget *parent) :
	QWidget(parent),
	_viewModel(static_cast<TabItemViewModel*>(viewModel)),
	ui(new Ui::TabItemView)
{
	ui->setupUi(this);

	QtMvvm::bind(_viewModel, "title",
				 ui->checkBox, "text",
				 QtMvvm::Binding::OneWayToView);
}

TabItemView::~TabItemView()
{
	delete ui;
}

TabItemViewModel *TabItemView::viewModel() const
{
	return _viewModel;
}

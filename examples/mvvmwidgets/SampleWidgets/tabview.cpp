#include "tabview.h"
#include "ui_tabview.h"
#include "tabitemview.h"
#include <QTabBar>

TabView::TabView(QtMvvm::ViewModel *viewModel, QWidget *parent) :
	QTabWidget(parent),
	IPresentingView(),
	_viewModel(static_cast<TabViewModel*>(viewModel)),
	ui(new Ui::TabView)
{
	setWindowFlags(windowFlags() | Qt::Window);
	ui->setupUi(this);

	tabBar()->addTab(QStringLiteral("+"));

	connect(tabBar(), &QTabBar::tabBarClicked,
			this, &TabView::trigger);
}

TabView::~TabView()
{
	delete ui;
}

bool TabView::tryPresent(QWidget *view)
{
	auto itemView = qobject_cast<TabItemView*>(view);
	if(itemView) {
		insertTab(count() - 1, itemView, itemView->viewModel()->title());
		tabBar()->setCurrentIndex(tabBar()->count() - 2);
		return true;
	} else
		return true;
}

void TabView::trigger(int index)
{
	tabBar()->setCurrentIndex(tabBar()->count() - 1);
	if(index == tabBar()->count() - 1)
		_viewModel->addTab();
}

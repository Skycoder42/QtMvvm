#include "drawerviewmodel.h"
#include <QtMvvmSettingsCore/SettingsViewModel>

#include "sampleviewmodel.h"
#include "tabviewmodel.h"

DrawerViewModel::DrawerViewModel(QObject *parent) :
	ViewModel(parent),
	_navModel(new QStandardItemModel(0, 1, this))
{
	_navModel->appendRow(new QStandardItem(tr("Main Sample")));
	_navModel->appendRow(new QStandardItem(tr("Tab Sample")));
	_navModel->appendRow(new QStandardItem(tr("Settings")));
}

DrawerViewModel::~DrawerViewModel()
{
	qInfo(Q_FUNC_INFO);
}

QStandardItemModel *DrawerViewModel::navModel() const
{
	return _navModel;
}

void DrawerViewModel::open(int index)
{
	switch (index) {
	case 0:
		show<SampleViewModel>();
		break;
	case 1:
		show<TabViewModel>();
		break;
	case 2:
		show<QtMvvm::SettingsViewModel>();
		break;
	default:
		break;
	}
}

#include "drawerviewmodel.h"
#include <QtMvvmCore/SettingsViewModel>

#include "sampleviewmodel.h"
#include "tabviewmodel.h"
#include "containerviewmodel.h"

DrawerViewModel::DrawerViewModel(QObject *parent) :
	ViewModel(parent),
	_navModel(new QStandardItemModel(0, 1, this))
{
	_navModel->appendRow(new QStandardItem(tr("Main Sample")));
	_navModel->appendRow(new QStandardItem(tr("Tab Sample")));
	_navModel->appendRow(new QStandardItem(tr("View Container Sample")));
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
		show<TabItemViewModel>({
								   {QStringLiteral("title"), QStringLiteral("Root Tab")}
							   });
		break;
	case 2:
		show<ContainerViewModel>();
		break;
	case 3:
		show<QtMvvm::SettingsViewModel>();
		break;
	default:
		break;
	}
}

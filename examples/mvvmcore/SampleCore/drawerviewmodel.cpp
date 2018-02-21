#include "drawerviewmodel.h"

#include "sampleviewmodel.h"

DrawerViewModel::DrawerViewModel(QObject *parent) :
	ViewModel(parent),
	_navModel(new QStandardItemModel(0, 1, this))
{
	_navModel->appendRow(new QStandardItem(tr("Main Sample")));
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
	default:
		break;
	}
}

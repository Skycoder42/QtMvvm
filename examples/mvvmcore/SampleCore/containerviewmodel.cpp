#include "containerviewmodel.h"

ContainerViewModel::ContainerViewModel(QObject *parent) :
	ViewModel(parent)
{}

ContainerViewModel::~ContainerViewModel()
{
	qInfo(Q_FUNC_INFO);
}

QString ContainerViewModel::vmType() const
{
	return QString::fromUtf8(QMetaType::typeName(qMetaTypeId<ChildViewModel*>()));
}

void ContainerViewModel::loadChild()
{
	show<ChildViewModel>();
}



ChildViewModel::ChildViewModel(QObject *parent) :
	ViewModel(parent)
{}

ChildViewModel::~ChildViewModel()
{
	qInfo(Q_FUNC_INFO);
}

#include "viewmodel.h"
#include "viewmodel_p.h"
using namespace QtMvvm;

ViewModel::ViewModel(QObject *parent) :
	QObject(parent),
	d(new ViewModelPrivate())
{}

ViewModel::~ViewModel() {}

ViewModel *ViewModel::parentViewModel() const
{
	return qobject_cast<ViewModel*>(parent());
}

bool ViewModel::deleteOnClose() const
{
	return d->deleteOnClose;
}

void ViewModel::setDeleteOnClose(bool deleteOnClose)
{
	if (d->deleteOnClose == deleteOnClose)
		return;

	d->deleteOnClose = deleteOnClose;
	emit deleteOnCloseChanged(deleteOnClose, {});
}

void ViewModel::onInit() {}

void ViewModel::onDestroy() {}

void ViewModel::onShow() {}

void ViewModel::onClose() {}

// ------------- Private Implementation -------------

ViewModelPrivate::ViewModelPrivate() :
	deleteOnClose(true)
{}

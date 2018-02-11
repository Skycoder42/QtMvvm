#include "viewmodel.h"
#include "viewmodel_p.h"
#include "coreapp_p.h"
using namespace QtMvvm;

ViewModel::ViewModel(QObject *parent) :
	QObject(parent),
	d(new ViewModelPrivate())
{}

ViewModel::~ViewModel() {}

ViewModel *ViewModel::parentViewModel() const
{
	return qobject_cast<ViewModel*>(parent()); //TODO not working that way, parent is always the view...
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

void ViewModel::onInit(const QVariantHash &) {}

void ViewModel::onDestroy() {}

void ViewModel::onShow() {}

void ViewModel::onClose() {}

void ViewModel::updateVisible(bool visible)
{
	if(visible)
		onShow();
	else
		onClose();
}

void ViewModel::showImp(const QMetaObject *mo, const QVariantHash &params, QPointer<ViewModel> parent)
{
	CoreAppPrivate::dInstance()->showViewModel(mo, params, parent);
}

// ------------- Private Implementation -------------

ViewModelPrivate::ViewModelPrivate() :
	deleteOnClose(true)
{}

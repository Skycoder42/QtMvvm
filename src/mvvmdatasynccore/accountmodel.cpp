#include "accountmodel.h"
#include "accountmodel_p.h"

#include <QRemoteObjectReplica>

using namespace QtMvvm;
using namespace QtDataSync;

AccountModel::AccountModel(QObject *parent) :
	QAbstractListModel(parent),
	d(new AccountModelPrivate())
{}

AccountModel::~AccountModel() {}

void AccountModel::setup(AccountManager *manager)
{
	beginResetModel();
	d->devices.clear();
	if(d->manager)
		d->manager->disconnect(this);
	d->manager = manager;
	endResetModel();

	connect(d->manager, &AccountManager::accountDevices,
			this, &AccountModel::accountDevices);
	connect(d->manager, &AccountManager::importAccepted,
			this, &AccountModel::reload);
	connect(d->manager, &AccountManager::accountAccessGranted,
			this, &AccountModel::reload);

	if(d->manager->replica()->isInitialized())
		d->manager->listDevices();
	else {
		connect(d->manager->replica(), &QRemoteObjectReplica::initialized,
				d->manager, &AccountManager::listDevices);
	}

}

QVariant AccountModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation != Qt::Horizontal)
		return {};

	switch (section) {
	case 0:
		switch(role) {
		case NameRole:
			return tr("Name");
		case FingerPrintRole:
			return tr("Fingerprint");
		default:
			break;
		}
		break;
	case 1:
		if(role == Qt::DisplayRole)
			return tr("Fingerprint");
		break;
	default:
		break;
	}

	return {};
}

int AccountModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	else
		return d->devices.size();
}

int AccountModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	else
		return 2;
}

QVariant AccountModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	switch (index.column()) {
	case 0:
		switch(role) {
		case NameRole:
			return d->devices.value(index.row()).name();
		case FingerPrintRole:
			return d->devices.value(index.row()).fingerprint();
		default:
			break;
		}
		break;
	case 1:
		if(role == Qt::DisplayRole)
			return d->devices.value(index.row()).fingerprint();
		break;
	default:
		break;
	}

	return {};
}

bool AccountModel::removeDevice(const QModelIndex &index)
{
	if (!index.isValid())
		return false;
	else {
		d->manager->removeDevice(d->devices.value(index.row()));
		return true;
	}
}

void AccountModel::reload()
{
	if(d->manager)
		d->manager->listDevices();
}

void AccountModel::accountDevices(const QList<DeviceInfo> &devices)
{
	beginResetModel();
	d->devices = devices;
	endResetModel();
}

// ------------- Private Implementation -------------

AccountModelPrivate::AccountModelPrivate() :
	manager(nullptr),
	devices()
{}

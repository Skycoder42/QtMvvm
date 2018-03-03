#include "exchangedevicesmodel.h"
#include "exchangedevicesmodel_p.h"
using namespace QtMvvm;
using namespace QtDataSync;

ExchangeDevicesModel::ExchangeDevicesModel(QObject *parent) :
	QAbstractListModel(parent),
	d(new ExchangeDevicesModelPrivate())
{}

ExchangeDevicesModel::~ExchangeDevicesModel() {}

void ExchangeDevicesModel::setup(QtDataSync::UserExchangeManager *exchangeManager)
{
	if(d->exchangeManager)
		d->exchangeManager->disconnect(this);

	beginResetModel();
	d->exchangeManager = exchangeManager;
	d->devices = d->exchangeManager->devices();
	endResetModel();

	connect(d->exchangeManager, &UserExchangeManager::devicesChanged,
			this, &ExchangeDevicesModel::updateDevices);
}

UserInfo ExchangeDevicesModel::infoAt(int index) const
{
	return d->devices.value(index);
}

QVariant ExchangeDevicesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation != Qt::Horizontal)
		return {};

	switch (section) {
	case 0:
		switch(role) {
		case NameRole:
			return tr("Name");
		case HostRole:
			return tr("Host");
		case PortRole:
			return tr("Port");
		case AddressRole:
			return tr("Address");
		default:
			break;
		}
		break;
	case 1:
		if(role == Qt::DisplayRole)
			return tr("Address");
		break;
	default:
		break;
	}

	return {};
}

int ExchangeDevicesModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	else
		return d->devices.size();
}

int ExchangeDevicesModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	else
		return 2;
}

QVariant ExchangeDevicesModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	switch (index.column()) {
	case 0:
		switch(role) {
		case NameRole:
			return d->devices.value(index.row()).name();
		case HostRole:
			return d->devices.value(index.row()).address().toString();
		case PortRole:
			return d->devices.value(index.row()).port();
		case AddressRole:
			return fullAddress(d->devices.value(index.row()));
		default:
			break;
		}
		break;
	case 1:
		if(role == Qt::DisplayRole)
			return fullAddress(d->devices.value(index.row()));
		break;
	default:
		break;
	}

	return {};
}

QHash<int, QByteArray> ExchangeDevicesModel::roleNames() const
{
	return {
		{NameRole, "name"},
		{HostRole, "host"},
		{PortRole, "port"},
		{AddressRole, "address"}
	};
}

QString ExchangeDevicesModel::fullAddress(const UserInfo &userInfo)
{
	return tr("%1:%2")
			.arg(userInfo.address().toString())
			.arg(userInfo.port());
}

void ExchangeDevicesModel::updateDevices(const QList<UserInfo> &devices)
{
	QList<UserInfo> addList;
	for(auto device : devices) {
		auto dIndex = d->devices.indexOf(device);
		if(dIndex != -1) {
			if(device.name() != d->devices[dIndex].name()) {
				d->devices[dIndex] = device;
				emit dataChanged(index(dIndex), index(dIndex, 1));
			}
		} else
			addList.append(device);
	}

	if(addList.isEmpty())
		return;
	beginInsertRows(QModelIndex(),
					d->devices.size(),
					d->devices.size() + addList.size() - 1);
	d->devices.append(addList);
	endInsertRows();
}

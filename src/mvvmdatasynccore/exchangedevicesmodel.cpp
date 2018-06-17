#include "exchangedevicesmodel.h"
#include "exchangedevicesmodel_p.h"
#include <chrono>

#undef logDebug
#undef logInfo
#undef logWarning
#undef logCritical
#include <QtMvvmCore/private/qtmvvm_logging_p.h>

using namespace QtMvvm;
using namespace QtDataSync;
using namespace std::chrono;

#if QT_HAS_INCLUDE(<chrono>)
#define chrtime(x) x
#else
#define chrtime(x) duration_cast<milliseconds>(x).count()
#endif

ExchangeDevicesModel::ExchangeDevicesModel(QObject *parent) :
	QAbstractListModel(parent),
	d(new ExchangeDevicesModelPrivate())
{}

ExchangeDevicesModel::~ExchangeDevicesModel() = default;

void ExchangeDevicesModel::setup(QtDataSync::UserExchangeManager *exchangeManager)
{
	if(d->exchangeManager)
		d->exchangeManager->disconnect(this);

	beginResetModel();
	d->exchangeManager = exchangeManager;
	d->devices.clear();
	d->devices.reserve(d->exchangeManager->devices().size());
	for(const auto &device : d->exchangeManager->devices())
		d->devices.append(device);
	endResetModel();

	connect(d->exchangeManager, &UserExchangeManager::devicesChanged,
			this, &ExchangeDevicesModel::updateDevices);
}

UserInfo ExchangeDevicesModel::infoAt(const QModelIndex &index) const
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
	if(!checkIndex(index, QAbstractItemModel::CheckIndexOption::ParentIsInvalid | QAbstractItemModel::CheckIndexOption::IndexIsValid))
		return {};
#else
	if(!index.isValid())
		return {};
#endif
	else
		return d->devices.value(index.row());
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
	if(!checkIndex(parent, QAbstractItemModel::CheckIndexOption::ParentIsInvalid))
		return 0;
#endif
	if (parent.isValid())
		return 0;
	else
		return d->devices.size();
}

int ExchangeDevicesModel::columnCount(const QModelIndex &parent) const
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
	if(!checkIndex(parent, QAbstractItemModel::CheckIndexOption::ParentIsInvalid))
		return 0;
#endif
	if (parent.isValid())
		return 0;
	else
		return 2;
}

QVariant ExchangeDevicesModel::data(const QModelIndex &index, int role) const
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
	if(!checkIndex(index, QAbstractItemModel::CheckIndexOption::ParentIsInvalid | QAbstractItemModel::CheckIndexOption::IndexIsValid))
		return {};
#else
	if(!index.isValid())
		return {};
#endif

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
	//find new devices and update existing
	QList<ExchangeDevicesModelPrivate::LimitedUserInfo> addList;
	addList.reserve(devices.size());
	for(const auto &device : devices) {
		auto dIndex = d->devices.indexOf(device);
		if(dIndex != -1) {
			if(device.name() != d->devices[dIndex].name()) {
				logDebug() << "Updating name of device" << device;
				d->devices[dIndex] = device;
				emit dataChanged(index(dIndex), index(dIndex, 1));
			} else
				d->devices[dIndex].deadline.setRemainingTime(chrtime(seconds(5)), Qt::VeryCoarseTimer);
		} else {
			logDebug() << "Adding new device" << device;
			addList.append(device);
		}
	}

	//remove old devices
	for(auto i = 0; i < d->devices.size();) {
		if(d->devices[i].deadline.hasExpired()) {
			logDebug() << "Removing stale device" << d->devices[i];
			beginRemoveRows({}, i, i);
			d->devices.removeAt(i);
			endRemoveRows();
		} else
			i++;
	}

	//add new devices
	if(addList.isEmpty())
		return;
	beginInsertRows({},
					d->devices.size(),
					d->devices.size() + addList.size() - 1);
	d->devices.append(addList);
	endInsertRows();
}



ExchangeDevicesModelPrivate::LimitedUserInfo::LimitedUserInfo(const UserInfo &info) :
	UserInfo(info),
	deadline(chrtime(seconds(5)), Qt::VeryCoarseTimer)
{}

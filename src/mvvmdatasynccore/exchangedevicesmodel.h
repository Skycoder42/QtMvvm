#ifndef QTMVVM_EXCHANGEDEVICESMODEL_H
#define QTMVVM_EXCHANGEDEVICESMODEL_H

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qscopedpointer.h>

#include <QtDataSync/userexchangemanager.h>

#include "QtMvvmDataSyncCore/qtmvvmdatasynccore_global.h"

namespace QtMvvm {

class ExchangeDevicesModelPrivate;
//! A list model implementation to show all devices available by the QtDataSync::UserExchangeManager
class Q_MVVMDATASYNCCORE_EXPORT ExchangeDevicesModel : public QAbstractListModel
{
	Q_OBJECT

public:
	//! The model roles this model provides
	enum Roles {
		NameRole = Qt::DisplayRole, //!< The local name of the device. The role name is `name`
		HostRole = Qt::UserRole + 1, //!< The host name (IP address) of the device. The role name is `host`
		PortRole, //!< The port the device is bound to.  The role name is `port`
		AddressRole //!< A full, readable address of the device (Host+Port). The role name is `address`
	};
	Q_ENUM(Roles)

	//! Default constructor
	explicit ExchangeDevicesModel(QObject *parent = nullptr);
	~ExchangeDevicesModel() override;

	//! Setups up the model to use the exchange manager. Automatically displays all devices of the manager
	Q_INVOKABLE void setup(QtDataSync::UserExchangeManager *exchangeManager);

	//! Returns the user info at the given index
	Q_INVOKABLE QtDataSync::UserInfo infoAt(const QModelIndex &index) const;
	//! @copybrief infoAt(const QModelIndex &) const
	Q_INVOKABLE inline QtDataSync::UserInfo infoAt(int row) const {
		return infoAt(index(row));
	}

	//! @inherit{QAbstractListModel::headerData}
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	//! @inherit{QAbstractListModel::rowCount}
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	//! @inherit{QAbstractListModel::columnCount}
	int columnCount(const QModelIndex &parent) const override;
	//! @inherit{QAbstractListModel::data}
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	//! @inherit{QAbstractListModel::roleNames}
	QHash<int, QByteArray> roleNames() const override;

	//! returns a localized string of the user infos address and port, combined to one
	static QString fullAddress(const QtDataSync::UserInfo &userInfo);

private Q_SLOTS:
	void updateDevices(const QList<QtDataSync::UserInfo> &devices);

private:
	QScopedPointer<ExchangeDevicesModelPrivate> d;
};

}

#endif // QTMVVM_EXCHANGEDEVICESMODEL_H

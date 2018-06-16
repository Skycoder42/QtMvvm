#ifndef QTMVVM_ACCOUNTMODEL_H
#define QTMVVM_ACCOUNTMODEL_H

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qscopedpointer.h>

#include <QtDataSync/accountmanager.h>
#include <QtDataSync/syncmanager.h>

#include "QtMvvmDataSyncCore/qtmvvmdatasynccore_global.h"

namespace QtMvvm {

class AccountModelPrivate;
//! A list model implementation for QtDataSync::DeviceInfo via a QtDataSync::AccountManager
class Q_MVVMDATASYNCCORE_EXPORT AccountModel : public QAbstractListModel
{
	Q_OBJECT

public:
	//! The model roles this model provides
	enum Roles {
		NameRole = Qt::DisplayRole, //!< The users name. The role name is `name`
		FingerPrintRole = Qt::UserRole + 1 //!< The cryptopgraphic fingerprint of the users keys. The role name is `fingerPrint`
	};
	Q_ENUM(Roles)

	//! Default constructor
	explicit AccountModel(QObject *parent = nullptr);
	~AccountModel() override;

	//! Setups up the model to use the account and a sync manager. Automatically loads devices
	Q_INVOKABLE void setup(QtDataSync::AccountManager *accountManager,
						   QtDataSync::SyncManager *syncManager);

	//! @inherit{QAbstractListModel::headerData}
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	//! @inherit{QAbstractListModel::rowCount}
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	//! @inherit{QAbstractListModel::columnCount}
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	//! @inherit{QAbstractListModel::data}
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	//! @inherit{QAbstractListModel::roleNames}
	QHash<int, QByteArray> roleNames() const override;

	//! Removes the device with the given index from the account
	Q_INVOKABLE bool removeDevice(const QModelIndex &index);
	//! @copybrief removeDevice(const QModelIndex &)
	Q_INVOKABLE inline bool removeDevice(int row) {
		return removeDevice(index(row));
	}

public Q_SLOTS:
	//! Reload the device list
	void reload();

private Q_SLOTS:
	void accountDevices(const QList<QtDataSync::DeviceInfo> &devices);
	void update(QtDataSync::SyncManager::SyncState state);

private:
	QScopedPointer<AccountModelPrivate> d;
};

}

#endif // QTMVVM_ACCOUNTMODEL_H

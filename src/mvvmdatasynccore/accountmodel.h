#ifndef QTMVVM_ACCOUNTMODEL_H
#define QTMVVM_ACCOUNTMODEL_H

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qscopedpointer.h>

#include <QtDataSync/accountmanager.h>

#include "QtMvvmDataSyncCore/qtmvvmdatasynccore_global.h"

namespace QtMvvm {

class AccountModelPrivate;
class Q_MVVMDATASYNCCORE_EXPORT AccountModel : public QAbstractListModel
{
	Q_OBJECT

public:
	enum Roles {
		NameRole = Qt::DisplayRole,
		FingerPrintRole = Qt::UserRole + 1
	};
	Q_ENUM(Roles)

	explicit AccountModel(QObject *parent = nullptr);
	~AccountModel();

	Q_INVOKABLE void setup(QtDataSync::AccountManager *manager);

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	Q_INVOKABLE bool removeDevice(const QModelIndex &index);
	Q_INVOKABLE inline bool removeDevice(int row) {
		return removeDevice(index(row));
	}

public Q_SLOTS:
	void reload();

private Q_SLOTS:
	void accountDevices(const QList<QtDataSync::DeviceInfo> &devices);

private:
	QScopedPointer<AccountModelPrivate> d;
};

}

#endif // QTMVVM_ACCOUNTMODEL_H

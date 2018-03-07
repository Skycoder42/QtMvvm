#ifndef QTMVVM_EXCHANGEDEVICESMODEL_H
#define QTMVVM_EXCHANGEDEVICESMODEL_H

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qscopedpointer.h>

#include <QtDataSync/userexchangemanager.h>

#include "QtMvvmDataSyncCore/qtmvvmdatasynccore_global.h"

namespace QtMvvm {

class ExchangeDevicesModelPrivate;
class Q_MVVMDATASYNCCORE_EXPORT ExchangeDevicesModel : public QAbstractListModel
{
	Q_OBJECT

public:
	enum Roles {
		NameRole = Qt::DisplayRole,
		HostRole = Qt::UserRole + 1,
		PortRole,
		AddressRole
	};
	Q_ENUM(Roles)

	explicit ExchangeDevicesModel(QObject *parent = nullptr);
	~ExchangeDevicesModel();

	Q_INVOKABLE void setup(QtDataSync::UserExchangeManager *exchangeManager);

	Q_INVOKABLE QtDataSync::UserInfo infoAt(const QModelIndex &index) const;
	Q_INVOKABLE inline QtDataSync::UserInfo infoAt(int row) const {
		return infoAt(index(row));
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;

	static QString fullAddress(const QtDataSync::UserInfo &userInfo);

private Q_SLOTS:
	void updateDevices(const QList<QtDataSync::UserInfo> &devices);

private:
	QScopedPointer<ExchangeDevicesModelPrivate> d;
};

}

#endif // QTMVVM_EXCHANGEDEVICESMODEL_H

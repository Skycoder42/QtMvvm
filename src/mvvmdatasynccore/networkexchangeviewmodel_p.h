#ifndef QTMVVM_NETWORKEXCHANGEVIEWMODEL_P_H
#define QTMVVM_NETWORKEXCHANGEVIEWMODEL_P_H

#include <QtDataSync/UserExchangeManager>

#include "qtmvvmdatasynccore_global.h"
#include "networkexchangeviewmodel.h"

namespace QtMvvm {

class NetworkExchangeViewModelPrivate
{
	Q_DISABLE_COPY(NetworkExchangeViewModelPrivate)

public:
	static const quint32 ExportRequestCode = 0xb300;

	NetworkExchangeViewModelPrivate(NetworkExchangeViewModel *q_ptr);

	QtDataSync::UserExchangeManager *exchangeManager = nullptr;
	ExchangeDevicesModel *deviceModel;
	QSortFilterProxyModel *sortedModel;
	quint16 port = QtDataSync::UserExchangeManager::DataExchangePort;
	QHash<quint32, QtDataSync::UserInfo> activeExports;

	bool active = false; //TODO remove once fixed in datasync
};

}

#endif // QTMVVM_NETWORKEXCHANGEVIEWMODEL_P_H

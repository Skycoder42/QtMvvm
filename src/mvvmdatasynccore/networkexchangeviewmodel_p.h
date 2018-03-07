#ifndef QTMVVM_NETWORKEXCHANGEVIEWMODEL_P_H
#define QTMVVM_NETWORKEXCHANGEVIEWMODEL_P_H

#include "qtmvvmdatasynccore_global.h"
#include "networkexchangeviewmodel.h"

namespace QtMvvm {

class NetworkExchangeViewModelPrivate
{
public:
	static const quint32 ExportRequestCode = 0xb300;

	NetworkExchangeViewModelPrivate(NetworkExchangeViewModel *q_ptr);

	QtDataSync::UserExchangeManager *exchangeManager;
	ExchangeDevicesModel *deviceModel;
	QSortFilterProxyModel *sortedModel;
	quint16 port;
	QHash<quint32, QtDataSync::UserInfo> activeExports;
};

}

#endif // QTMVVM_NETWORKEXCHANGEVIEWMODEL_P_H

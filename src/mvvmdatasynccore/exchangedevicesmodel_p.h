#ifndef QTMVVM_EXCHANGEDEVICESMODEL_P_H
#define QTMVVM_EXCHANGEDEVICESMODEL_P_H

#include <QtCore/QHash>

#include "qtmvvmdatasynccore_global.h"
#include "exchangedevicesmodel.h"

namespace QtMvvm {

class ExchangeDevicesModelPrivate
{
public:
	QtDataSync::UserExchangeManager *exchangeManager = nullptr;
	QList<QtDataSync::UserInfo> devices;
};

}

#endif // QTMVVM_EXCHANGEDEVICESMODEL_P_H

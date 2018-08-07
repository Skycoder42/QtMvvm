#ifndef QTMVVM_EXCHANGEDEVICESMODEL_P_H
#define QTMVVM_EXCHANGEDEVICESMODEL_P_H

#include <QtCore/QHash>
#include <QtCore/QDeadlineTimer>

#include "qtmvvmdatasynccore_global.h"
#include "exchangedevicesmodel.h"

namespace QtMvvm {

class ExchangeDevicesModelPrivate
{
	Q_DISABLE_COPY(ExchangeDevicesModelPrivate)

public:
	ExchangeDevicesModelPrivate() = default;

	struct LimitedUserInfo : public QtDataSync::UserInfo
	{
		LimitedUserInfo(const QtDataSync::UserInfo &info = {});

		QDeadlineTimer deadline;
	};

	QtDataSync::UserExchangeManager *exchangeManager = nullptr;
	QList<LimitedUserInfo> devices;
};

}

#endif // QTMVVM_EXCHANGEDEVICESMODEL_P_H

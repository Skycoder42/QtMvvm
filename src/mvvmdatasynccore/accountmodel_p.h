#ifndef QTMVVM_ACCOUNTMODEL_P_H
#define QTMVVM_ACCOUNTMODEL_P_H

#include "qtmvvmdatasynccore_global.h"
#include "accountmodel.h"

namespace QtMvvm {

class AccountModelPrivate
{
	Q_DISABLE_COPY(AccountModelPrivate)

public:
	AccountModelPrivate() = default;

	QtDataSync::AccountManager *accountManager = nullptr;
	QtDataSync::SyncManager *syncManager = nullptr;
	QList<QtDataSync::DeviceInfo> devices;
	bool reloaded = true;
};

}

#endif // QTMVVM_ACCOUNTMODEL_P_H

#ifndef QTMVVM_ACCOUNTMODEL_P_H
#define QTMVVM_ACCOUNTMODEL_P_H

#include "qtmvvmdatasynccore_global.h"
#include "accountmodel.h"

namespace QtMvvm {

class AccountModelPrivate
{
public:
	AccountModelPrivate();

	QtDataSync::AccountManager *accountManager;
	QtDataSync::SyncManager *syncManager;
	QList<QtDataSync::DeviceInfo> devices;
	bool reloaded;
};

}

#endif // QTMVVM_ACCOUNTMODEL_P_H

#ifndef QTMVVM_DATASYNCSETTINGSACCESSOR_P_H
#define QTMVVM_DATASYNCSETTINGSACCESSOR_P_H

#include "qtmvvmdatasynccore_global.h"
#include "datasyncsettingsaccessor.h"

namespace QtMvvm {

class DataSyncSettingsAccessorPrivate
{
public:
	DataSyncSettingsAccessorPrivate(QtDataSync::DataTypeStore<DataSyncSettingsEntry> *store);

	QtDataSync::DataTypeStore<DataSyncSettingsEntry> *store;
};

}

#endif // QTMVVM_DATASYNCSETTINGSACCESSOR_P_H

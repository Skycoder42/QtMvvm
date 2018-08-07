#ifndef QTMVVM_DATASYNCSETTINGSACCESSOR_P_H
#define QTMVVM_DATASYNCSETTINGSACCESSOR_P_H

#include "qtmvvmdatasynccore_global.h"
#include "datasyncsettingsaccessor.h"

namespace QtMvvm {

class DataSyncSettingsAccessorPrivate
{
	Q_DISABLE_COPY(DataSyncSettingsAccessorPrivate)

public:
	DataSyncSettingsAccessorPrivate(QtDataSync::DataTypeStore<DataSyncSettingsEntry> *store);

	QtDataSync::DataTypeStore<DataSyncSettingsEntry> *store;
};

}

#endif // QTMVVM_DATASYNCSETTINGSACCESSOR_P_H

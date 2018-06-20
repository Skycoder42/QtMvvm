#ifndef QTMVVM_DATASYNCSETTINGSVIEWMODEL_P_H
#define QTMVVM_DATASYNCSETTINGSVIEWMODEL_P_H

#include "qtmvvmdatasynccore_global.h"
#include "datasyncsettingsviewmodel.h"

#include <QtDataSync/DataTypeStore>

namespace QtMvvm {

class DataSyncSettingsViewModelPrivate
{
public:
	DataSyncSettingsViewModelPrivate(DataSyncSettingsViewModel *q_ptr);

	QtDataSync::DataTypeStore<DataSyncSettingsEntry> *store;
};

}

#endif // QTMVVM_DATASYNCSETTINGSVIEWMODEL_P_H

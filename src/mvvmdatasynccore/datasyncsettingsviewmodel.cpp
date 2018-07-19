#include "datasyncsettingsviewmodel.h"
#include "datasyncsettingsviewmodel_p.h"

#undef logDebug
#undef logInfo
#undef logWarning
#undef logCritical
#include <QtMvvmCore/private/qtmvvm_logging_p.h>

using namespace QtMvvm;

DataSyncSettingsViewModel::DataSyncSettingsViewModel(QObject *parent) :
	SettingsViewModel{parent},
	d(new DataSyncSettingsViewModelPrivate{this})
{
	connect(d->store, &QtDataSync::DataTypeStoreBase::dataChanged,
			this, &DataSyncSettingsViewModel::valueChanged);
}

DataSyncSettingsViewModel::~DataSyncSettingsViewModel() = default;

QVariant DataSyncSettingsViewModel::loadValue(const QString &key, const QVariant &defaultValue) const
{
	try {
		return d->store->load(key).value();
	} catch (QtDataSync::NoDataException &e) {
		Q_UNUSED(e)
		return defaultValue;
	} catch (QException &e) {
		logCritical() << "Failed to load entry" << key << "from datasync settings with error:"
					  << e.what();
		return defaultValue;
	}
}

void DataSyncSettingsViewModel::saveValue(const QString &key, const QVariant &value)
{
	try {
		d->store->save({key, value});
	} catch (QException &e) {
		logCritical() << "Failed to save entry" << key << "to datasync settings with error:"
					  << e.what();
	}
}

void DataSyncSettingsViewModel::resetValue(const QString &key)
{
	try {
		d->store->remove(key);
	} catch (QException &e) {
		logCritical() << "Failed to remove entry" << key << "from datasync settings with error:"
					  << e.what();
	}
}

// ------------- Private Implementation -------------

DataSyncSettingsViewModelPrivate::DataSyncSettingsViewModelPrivate(DataSyncSettingsViewModel *q_ptr) :
	store{new QtDataSync::DataTypeStore<DataSyncSettingsEntry>{q_ptr}}
{}

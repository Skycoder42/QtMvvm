#include "datasyncsettingsviewmodel.h"
#include "datasyncsettingsaccessor.h"

#undef logDebug
#undef logInfo
#undef logWarning
#undef logCritical
#include <QtMvvmCore/private/qtmvvm_logging_p.h>

using namespace QtMvvm;

const QString DataSyncSettingsViewModel::paramSetup(QStringLiteral("setup"));
const QString DataSyncSettingsViewModel::paramDataStore(QStringLiteral("dataStore"));
const QString DataSyncSettingsViewModel::paramDataTypeStore(QStringLiteral("dataTypeStore"));

QVariantHash DataSyncSettingsViewModel::showParams(const QString &setup, const QString &setupFile)
{
	return {
		{paramSetup, setup},
		{paramSetupFile, setupFile}
	};
}

QVariantHash DataSyncSettingsViewModel::showParams(QtDataSync::DataStore *dataStore, const QString &setupFile)
{
	return {
		{paramDataStore, QVariant::fromValue(dataStore)},
		{paramSetupFile, setupFile}
	};
}

QVariantHash DataSyncSettingsViewModel::showParams(QtDataSync::DataTypeStore<DataSyncSettingsEntry> *dataStore, const QString &setupFile)
{
	return {
		{paramDataTypeStore, QVariant::fromValue<QtDataSync::DataTypeStoreBase*>(dataStore)},
		{paramSetupFile, setupFile}
	};
}

DataSyncSettingsViewModel::DataSyncSettingsViewModel(QObject *parent) :
	SettingsViewModel{parent}
{}

void DataSyncSettingsViewModel::onInit(const QVariantHash &params)
{
	if(!params.contains(paramAccessor)) {
		auto subParams = params;
		try {
			DataSyncSettingsAccessor *accessor = nullptr;
			auto setup = params.value(paramSetup).toString();
			if(!setup.isNull())
				accessor = new DataSyncSettingsAccessor{setup ,this};

			if(!accessor) {
				auto store = params.value(paramDataStore).value<QtDataSync::DataStore*>();
				if(store)
					accessor = new DataSyncSettingsAccessor{store ,this};
			}

			if(!accessor) {
				auto store = params.value(paramDataStore).value<QtDataSync::DataTypeStoreBase*>();
				auto tStore = dynamic_cast<QtDataSync::DataTypeStore<DataSyncSettingsEntry>*>(store);
				if(tStore)
					accessor = new DataSyncSettingsAccessor{tStore ,this};
			}

			if(!accessor)
				accessor = new DataSyncSettingsAccessor{this};

			subParams.insert(paramAccessor, QVariant::fromValue(accessor));
		} catch(QtDataSync::Exception &e) {
			logCritical() << "Failed to create DataSyncSettingsAccessor with error:" << e.what();
			// will be initialize with the default as fallback
		}
		SettingsViewModel::onInit(subParams);
	} else
		SettingsViewModel::onInit(params);
}

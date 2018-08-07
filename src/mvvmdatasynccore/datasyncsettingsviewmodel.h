#ifndef QTMVVM_DATASYNCSETTINGSVIEWMODEL_H
#define QTMVVM_DATASYNCSETTINGSVIEWMODEL_H

#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>
#include <QtDataSync/DataStore>
#include <QtDataSync/DataTypeStore>

#include <QtMvvmCore/settingsviewmodel.h>

#include "QtMvvmDataSyncCore/qtmvvmdatasynccore_global.h"
#include "QtMvvmDataSyncCore/datasyncsettingsentry.h"

namespace QtMvvm {

//! An extension of the normal settings viewmodel with the DataSyncSettingsAccessor as accessor
class Q_MVVMDATASYNCCORE_EXPORT DataSyncSettingsViewModel : public QtMvvm::SettingsViewModel
{
	Q_OBJECT

public:
	//! The parameter for a setup name for the onInit() method
	static const QString paramSetup;
	//! The parameter for a QtDataSync::DataStore object for the onInit() method
	static const QString paramDataStore;
	//! The parameter for a QtDataSync::DataTypeStore object for the onInit() method
	static const QString paramDataTypeStore;

	//! @copybrief SettingsViewModel::showParams(ISettingsAccessor*, const QString &)
	static QVariantHash showParams(const QString &setup, const QString &setupFile = {});
	//! @copybrief SettingsViewModel::showParams(ISettingsAccessor*, const QString &)
	static QVariantHash showParams(QtDataSync::DataStore *dataStore, const QString &setupFile = {});
	//! @copybrief SettingsViewModel::showParams(ISettingsAccessor*, const QString &)
	static QVariantHash showParams(QtDataSync::DataTypeStore<DataSyncSettingsEntry> *dataStore, const QString &setupFile = {});

	//! Invokable constructor
	Q_INVOKABLE explicit DataSyncSettingsViewModel(QObject *parent = nullptr);

protected:
	void onInit(const QVariantHash &params) override;
};

}

#endif // QTMVVM_DATASYNCSETTINGSVIEWMODEL_H

#include "datasyncsettingsaccessor.h"
#include "datasyncsettingsaccessor_p.h"
#include <QtCore/QRegularExpression>
#include <QtMvvmCore/exception.h>

#undef logDebug
#undef logInfo
#undef logWarning
#undef logCritical
#include <QtMvvmCore/private/qtmvvm_logging_p.h>

using namespace QtMvvm;

DataSyncSettingsAccessor::DataSyncSettingsAccessor(QObject *parent) :
	DataSyncSettingsAccessor{QtDataSync::DefaultSetup, parent}
{}

DataSyncSettingsAccessor::DataSyncSettingsAccessor(const QString &setupName, QObject *parent) :
	DataSyncSettingsAccessor{new QtDataSync::DataTypeStore<DataSyncSettingsEntry>{setupName}, parent}
{
	d->store->setParent(this);
}

DataSyncSettingsAccessor::DataSyncSettingsAccessor(QtDataSync::DataStore *store, QObject *parent) :
	DataSyncSettingsAccessor{new QtDataSync::DataTypeStore<DataSyncSettingsEntry>{store}, parent}
{
	d->store->setParent(this);
}

DataSyncSettingsAccessor::DataSyncSettingsAccessor(QtDataSync::DataTypeStore<DataSyncSettingsEntry> *store, QObject *parent) :
	ISettingsAccessor{parent},
	d{new DataSyncSettingsAccessorPrivate{store}}
{
	connect(d->store, &QtDataSync::DataTypeStoreBase::dataChanged,
			this, &DataSyncSettingsAccessor::dataChanged);
}

DataSyncSettingsAccessor::~DataSyncSettingsAccessor() = default;

bool DataSyncSettingsAccessor::contains(const QString &key) const
{
	try {
		return d->store->keys().contains(key);
	} catch (QTMVVM_EXCEPTION_BASE &e) {
		logCritical() << "Failed to check if entry" << key << "exists with error:"
					  << e.what();
		return false;
	}
}

QVariant DataSyncSettingsAccessor::load(const QString &key, const QVariant &defaultValue) const
{
	try {
		return d->store->load(key).value();
	} catch (QtDataSync::NoDataException &e) {
		Q_UNUSED(e)
		return defaultValue;
	} catch (QTMVVM_EXCEPTION_BASE &e) {
		logCritical() << "Failed to load entry" << key << "from datasync settings with error:"
					  << e.what();
		return defaultValue;
	}
}

void DataSyncSettingsAccessor::save(const QString &key, const QVariant &value)
{
	try {
		d->store->save({key, value});
	} catch (QTMVVM_EXCEPTION_BASE &e) {
		logCritical() << "Failed to save entry" << key << "to datasync settings with error:"
					  << e.what();
	}
}

void DataSyncSettingsAccessor::remove(const QString &key)
{
	try {
		auto rmKeys = d->store->keys().filter(QRegularExpression{
			QStringLiteral(R"__(^%1\/.*)__").arg(QRegularExpression::escape(key)),
			QRegularExpression::DontCaptureOption
		});
		for(const auto &rmKey : rmKeys)
			d->store->remove(rmKey);
		d->store->remove(key);
	} catch (QTMVVM_EXCEPTION_BASE &e) {
		logCritical() << "Failed to remove entry" << key << "from datasync settings with error:"
					  << e.what();
	}
}

void DataSyncSettingsAccessor::sync()
{
	// nothing needs to be done
	//TODO implement via manager sync with blocking eventloop?
}

void DataSyncSettingsAccessor::dataChanged(const QString &key, const QVariant &value)
{
	if(value.isValid())
		emit entryChanged(key, value.value<DataSyncSettingsEntry>().value());
	else
		emit entryRemoved(key);
}

// ------------- Private Implementation -------------

DataSyncSettingsAccessorPrivate::DataSyncSettingsAccessorPrivate(QtDataSync::DataTypeStore<DataSyncSettingsEntry> *store) :
	store{store}
{}

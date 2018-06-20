#include "datasyncsettingsviewmodel.h"
#include "datasyncsettingsviewmodel_p.h"
#include <QtCore/QDataStream>
#include <QtCore/QDebug>

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
		qCritical() << "Failed to save entry" << key << "to datasync settings with error:"
					<< e.what();
	}
}

void DataSyncSettingsViewModel::resetValue(const QString &key)
{
	try {
		d->store->remove(key);
	} catch (QException &e) {
		qCritical() << "Failed to remove entry" << key << "from datasync settings with error:"
					<< e.what();
	}
}



DataSyncSettingsEntry::DataSyncSettingsEntry() = default;

DataSyncSettingsEntry::DataSyncSettingsEntry(QString key, const QVariant &value) :
	_key{std::move(key)}
{
	setValue(value);
}

QString DataSyncSettingsEntry::key() const
{
	return _key;
}

QVariant DataSyncSettingsEntry::value() const
{
	QVariant value;
	QDataStream stream(_value);
	stream.setVersion(_dataVersion);
	stream >> value;
	return value;
}

void DataSyncSettingsEntry::setValue(const QVariant &value)
{
	_value.clear();
	QDataStream stream(&_value, QIODevice::WriteOnly);
	_dataVersion = stream.version();
	stream << value;
}

// ------------- Private Implementation -------------

DataSyncSettingsViewModelPrivate::DataSyncSettingsViewModelPrivate(DataSyncSettingsViewModel *q_ptr) :
	store{new QtDataSync::DataTypeStore<DataSyncSettingsEntry>{q_ptr}}
{}

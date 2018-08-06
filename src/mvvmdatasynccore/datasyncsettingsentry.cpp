#include "datasyncsettingsentry.h"
#include <QtCore/QDataStream>
#if QT_HAS_INCLUDE(<optional>) && __cplusplus >= 201703L
#include <optional>
#define QTMVVM_HAS_OPTIONAL
#endif

#undef logDebug
#undef logInfo
#undef logWarning
#undef logCritical
#include <QtMvvmCore/private/qtmvvm_logging_p.h>
using namespace QtMvvm;

namespace QtMvvm {

class DataSyncSettingsEntryData : public QSharedData
{
public:
	inline DataSyncSettingsEntryData() = default;
	inline DataSyncSettingsEntryData(QString key) :
		key{std::move(key)}
	{}

	QString key;
	int version = QDataStream::Qt_DefaultCompiledVersion;
	QByteArray data;
#ifdef QTMVVM_HAS_OPTIONAL
	mutable std::optional<QVariant> value;
#else
	mutable QVariant value;
#endif
};

}

DataSyncSettingsEntry::DataSyncSettingsEntry() :
	d{new DataSyncSettingsEntryData{}}
{}

DataSyncSettingsEntry::~DataSyncSettingsEntry() = default;

DataSyncSettingsEntry::DataSyncSettingsEntry(const DataSyncSettingsEntry &other) = default;

DataSyncSettingsEntry::DataSyncSettingsEntry(DataSyncSettingsEntry &&other) noexcept = default;

DataSyncSettingsEntry &DataSyncSettingsEntry::operator=(const DataSyncSettingsEntry &other) = default;

DataSyncSettingsEntry &DataSyncSettingsEntry::operator=(DataSyncSettingsEntry &&other) noexcept = default;

DataSyncSettingsEntry::DataSyncSettingsEntry(QString key, QVariant value) :
	d{new DataSyncSettingsEntryData{std::move(key)}}
{
	setValue(std::move(value));
}

QString DataSyncSettingsEntry::key() const
{
	return d->key;
}

int DataSyncSettingsEntry::dataVersion() const
{
	return d->version;
}

QVariant DataSyncSettingsEntry::value() const
{
#ifdef QTMVVM_HAS_OPTIONAL
	if(!d->data.isNull() && !d->value) {
		QDataStream stream{d->data};
		stream.setVersion(d->version);
		d->value = QVariant{};
		stream.startTransaction();
		stream >> d->value.value();
		if(!stream.commitTransaction())
			logWarning() << "Failed to read data of entry with key" << d->key;
	}
	return d->value.value_or(QVariant{});
#else
	if(!d->data.isNull() && !d->value.isValid()) {
		QDataStream stream{d->data};
		stream.setVersion(d->version);
		stream.startTransaction();
		stream >> d->value;
		if(!stream.commitTransaction())
			logWarning() << "Failed to read data of entry with key" << d->key;
	}
	return d->value;
#endif
}

void DataSyncSettingsEntry::setKey(QString key)
{
	d->key = std::move(key);
}

void DataSyncSettingsEntry::setDataVersion(int dataVersion)
{
	if(d->version == dataVersion)
		return;

	if(d->data.isNull())
		d->version = dataVersion;
	else {
		// update the internally stored data based on the version
		auto mValue = value();
		d->version = dataVersion;
		setValue(mValue);
	}
}

void DataSyncSettingsEntry::setValue(QVariant value)
{
#ifdef QTMVVM_HAS_OPTIONAL
	d->value.reset();
#else
	d->value.clear();
#endif
	d->data.clear();
	QDataStream stream{&d->data, QIODevice::WriteOnly};
	stream.setVersion(d->version);
	stream << value;
	d->value = std::move(value);
}

QByteArray DataSyncSettingsEntry::valueData() const
{
	return d->data;
}

void DataSyncSettingsEntry::setValueData(QByteArray data)
{
#ifdef QTMVVM_HAS_OPTIONAL
	d->value.reset();
#else
	d->value.clear();
#endif
	d->data = std::move(data);
}

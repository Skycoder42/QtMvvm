#ifndef QTMVVM_DATASYNCSETTINGSENTRY_H
#define QTMVVM_DATASYNCSETTINGSENTRY_H

#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>
#include <QtCore/qshareddata.h>

#include "QtMvvmDataSyncCore/qtmvvmdatasynccore_global.h"

namespace QtMvvm {

class DataSyncSettingsEntryData;
//! A class used with the DataSyncSettingsAccessor to store and sync generic settings
struct Q_MVVMDATASYNCCORE_EXPORT DataSyncSettingsEntry
{
	Q_GADGET

	//! They key of the entry stored
	Q_PROPERTY(QString key READ key WRITE setKey USER true)
	//! The QDataStream::Version that was used to serialize the entry
	Q_PROPERTY(int dataVersion READ dataVersion WRITE setDataVersion)
	//! The raw (deserialized) value stored
	Q_PROPERTY(QVariant value READ value WRITE setValue STORED false)

	//! @private Internal property
	Q_PROPERTY(QByteArray valueData READ valueData WRITE setValueData)

public:
	DataSyncSettingsEntry();
	~DataSyncSettingsEntry();
	//! Copy constructor
	DataSyncSettingsEntry(const DataSyncSettingsEntry &other);
	//! Move constructor
	DataSyncSettingsEntry(DataSyncSettingsEntry &&other) noexcept;
	//! Copy assignment operator
	DataSyncSettingsEntry &operator=(const DataSyncSettingsEntry &other);
	//! Move assignment operator
	DataSyncSettingsEntry &operator=(DataSyncSettingsEntry &&other) noexcept;

	//! Constructor, takes a key and value
	DataSyncSettingsEntry(QString key, QVariant value);
	//! @copybrief DataSyncSettingsEntry::DataSyncSettingsEntry(QString, QVariant)
	template <typename T>
	DataSyncSettingsEntry(QString key, const T &value);

	//! @readAcFn{key}
	QString key() const;
	//! @readAcFn{dataVersion}
	int dataVersion() const;

	//! @readAcFn{variantValue}
	QVariant value() const;
	//! @readAcFn{variantValue}
	template <typename T>
	T value() const;

	//! @writeAcFn{key}
	void setKey(QString key);
	//! @writeAcFn{dataVersion}
	void setDataVersion(int dataVersion);
	//! @writeAcFn{variantValue}
	void setValue(QVariant value);

private:
	QByteArray valueData() const;
	void setValueData(QByteArray value);

	QSharedDataPointer<DataSyncSettingsEntryData> d;
};

// ------------- Generic Implementation -------------

template<typename T>
DataSyncSettingsEntry::DataSyncSettingsEntry(QString key, const T &value) :
	DataSyncSettingsEntry{std::move(key), QVariant::fromValue(value)}
{}

template<typename T>
T DataSyncSettingsEntry::value() const
{
	return value().template value<T>();
}

}

Q_DECLARE_METATYPE(QtMvvm::DataSyncSettingsEntry)

#endif // QTMVVM_DATASYNCSETTINGSENTRY_H

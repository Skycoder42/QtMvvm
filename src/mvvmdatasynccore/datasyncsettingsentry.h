#ifndef QTMVVM_DATASYNCSETTINGSENTRY_H
#define QTMVVM_DATASYNCSETTINGSENTRY_H

#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>
#include <QtCore/qshareddata.h>

#include "QtMvvmDataSyncCore/qtmvvmdatasynccore_global.h"

namespace QtMvvm {

class DataSyncSettingsEntryData;
struct Q_MVVMDATASYNCCORE_EXPORT DataSyncSettingsEntry
{
	Q_GADGET

	Q_PROPERTY(QString key READ key WRITE setKey USER true)
	Q_PROPERTY(int dataVersion READ dataVersion WRITE setDataVersion)
	Q_PROPERTY(QByteArray value READ valueData WRITE setValueData)

	Q_PROPERTY(QVariant variantValue READ value WRITE setValue STORED false)

public:
	DataSyncSettingsEntry();
	~DataSyncSettingsEntry();
	DataSyncSettingsEntry(const DataSyncSettingsEntry &other);
	DataSyncSettingsEntry(DataSyncSettingsEntry &&other) noexcept;
	DataSyncSettingsEntry &operator=(const DataSyncSettingsEntry &other);
	DataSyncSettingsEntry &operator=(DataSyncSettingsEntry &&other) noexcept;

	DataSyncSettingsEntry(QString key, QVariant value);
	template <typename T>
	DataSyncSettingsEntry(QString key, const T &value);

	QString key() const;
	int dataVersion() const;

	QVariant value() const;
	template <typename T>
	T value() const;

	void setKey(QString key);
	void setDataVersion(int dataVersion);
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

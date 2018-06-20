#ifndef QTMVVM_DATASYNCSETTINGSVIEWMODEL_H
#define QTMVVM_DATASYNCSETTINGSVIEWMODEL_H

#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>

#include <QtMvvmCore/settingsviewmodel.h>

#include "QtMvvmDataSyncCore/qtmvvmdatasynccore_global.h"

namespace QtMvvm {

struct Q_MVVMDATASYNCCORE_EXPORT DataSyncSettingsEntry
{
	Q_GADGET

	Q_PROPERTY(QString key MEMBER _key USER true)
	Q_PROPERTY(int dataVersion MEMBER _dataVersion)
	Q_PROPERTY(QByteArray value MEMBER _value)

	Q_PROPERTY(QVariant varValue READ value WRITE setValue STORED false)

public:
	DataSyncSettingsEntry();
	DataSyncSettingsEntry(QString key, const QVariant &value);
	template <typename T>
	DataSyncSettingsEntry(QString key, const T &value);

	QString key() const;
	QVariant value() const;
	template <typename T>
	T value() const;

	void setValue(const QVariant &value);

private:
	QString _key;
	int _dataVersion = 0;
	QByteArray _value;
};

class DataSyncSettingsViewModelPrivate;
class Q_MVVMDATASYNCCORE_EXPORT DataSyncSettingsViewModel : public QtMvvm::SettingsViewModel
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit DataSyncSettingsViewModel(QObject *parent = nullptr);
	~DataSyncSettingsViewModel() override;

	QVariant loadValue(const QString &key, const QVariant &defaultValue) const override;
	void saveValue(const QString &key, const QVariant &value) override;
	void resetValue(const QString &key) override;

private:
	QScopedPointer<DataSyncSettingsViewModelPrivate> d;
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

#endif // QTMVVM_DATASYNCSETTINGSVIEWMODEL_H

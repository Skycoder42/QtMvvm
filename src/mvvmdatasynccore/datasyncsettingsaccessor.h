#ifndef QTMVVM_DATASYNCSETTINGSACCESSOR_H
#define QTMVVM_DATASYNCSETTINGSACCESSOR_H

#include <QtCore/qscopedpointer.h>
#include <QtMvvmCore/isettingsaccessor.h>
#include <QtDataSync/datatypestore.h>

#include "QtMvvmDataSyncCore/qtmvvmdatasynccore_global.h"
#include "QtMvvmDataSyncCore/datasyncsettingsentry.h"

namespace QtMvvm {

class DataSyncSettingsAccessorPrivate;
//! A settings accessor implementation that allows to store and sync settings via datasync
class Q_MVVMDATASYNCCORE_EXPORT DataSyncSettingsAccessor : public ISettingsAccessor
{
	Q_OBJECT
	Q_INTERFACES(QtMvvm::ISettingsAccessor)

public:
	//! Default Constructor
	Q_INVOKABLE explicit DataSyncSettingsAccessor(QObject *parent = nullptr);
	//! Constructor, with the name of the datasync setup to use
	explicit DataSyncSettingsAccessor(const QString &setupName, QObject *parent = nullptr);
	//! Constructor, with the store to use for access. Does not take ownership
	explicit DataSyncSettingsAccessor(QtDataSync::DataStore *store, QObject *parent = nullptr);
	//! Constructor, with the store to use for access. Does not take ownership
	explicit DataSyncSettingsAccessor(QtDataSync::DataTypeStore<DataSyncSettingsEntry> *store, QObject *parent = nullptr);
	~DataSyncSettingsAccessor() override;

	bool contains(const QString &key) const override;
	QVariant load(const QString &key, const QVariant &defaultValue) const override;
	void save(const QString &key, const QVariant &value) override;
	void remove(const QString &key) override;

public Q_SLOTS:
	//! @copydoc ISettingsAccessor::sync
	void sync() override;

private Q_SLOTS:
	void dataChanged(const QString &key, const QVariant &value);

private:
	QScopedPointer<DataSyncSettingsAccessorPrivate> d;
};

}

Q_DECLARE_METATYPE(QtMvvm::DataSyncSettingsAccessor*)

#endif // QTMVVM_DATASYNCSETTINGSACCESSOR_H

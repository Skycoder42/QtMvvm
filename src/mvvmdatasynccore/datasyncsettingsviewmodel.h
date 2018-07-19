#ifndef QTMVVM_DATASYNCSETTINGSVIEWMODEL_H
#define QTMVVM_DATASYNCSETTINGSVIEWMODEL_H

#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>

#include <QtMvvmCore/settingsviewmodel.h>

#include "QtMvvmDataSyncCore/qtmvvmdatasynccore_global.h"
#include "QtMvvmDataSyncCore/datasyncsettingsentry.h"

namespace QtMvvm {

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

}

#endif // QTMVVM_DATASYNCSETTINGSVIEWMODEL_H

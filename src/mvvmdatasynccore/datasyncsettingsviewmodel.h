#ifndef QTMVVM_DATASYNCSETTINGSVIEWMODEL_H
#define QTMVVM_DATASYNCSETTINGSVIEWMODEL_H

#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>

#include <QtMvvmCore/settingsviewmodel.h>

#include "QtMvvmDataSyncCore/qtmvvmdatasynccore_global.h"

namespace QtMvvm {

class Q_MVVMDATASYNCCORE_EXPORT DataSyncSettingsViewModel : public QtMvvm::SettingsViewModel
{
	Q_OBJECT

public:
	static const QString paramSetup;
	static const QString paramDataStore;
	static const QString paramDataTypeStore;

	Q_INVOKABLE explicit DataSyncSettingsViewModel(QObject *parent = nullptr);

protected:
	void onInit(const QVariantHash &params) override;
};

}

#endif // QTMVVM_DATASYNCSETTINGSVIEWMODEL_H

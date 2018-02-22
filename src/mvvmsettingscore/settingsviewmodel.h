#ifndef QTMVVM_SETTINGSVIEWMODEL_H
#define QTMVVM_SETTINGSVIEWMODEL_H

#include <QtCore/qsettings.h>
#include <QtCore/qscopedpointer.h>

#include <QtMvvmCore/viewmodel.h>

#include "QtMvvmSettingsCore/qtmvvmsettingscore_global.h"
#include "QtMvvmSettingsCore/settingssetuploader.h"

namespace QtMvvm {

class SettingsViewModelPrivate;
class SettingsViewModel : public ViewModel
{
	Q_OBJECT

	Q_PROPERTY(bool canRestoreDefaults READ canRestoreDefaults CONSTANT)

	Q_PROPERTY(QtMvvm::ISettingsSetupLoader* settingsSetupLoader READ settingsSetupLoader WRITE setSettingsSetupLoader NOTIFY settingsSetupLoaderChanged)
	QTMVVM_INJECT(QtMvvm::ISettingsSetupLoader*, settingsSetupLoader)

public:
	Q_INVOKABLE explicit SettingsViewModel(QObject *parent = nullptr);
	~SettingsViewModel();

	static void showSettings(ViewModel *parent);
	static void showSettings(QSettings *settings = nullptr, const QString &setupFile = {}, ViewModel *parent = nullptr);

	virtual bool canRestoreDefaults() const;

	QtMvvm::ISettingsSetupLoader* settingsSetupLoader() const;

public Q_SLOTS:
	virtual void callAction(const QString &entryId);

	void setSettingsSetupLoader(QtMvvm::ISettingsSetupLoader* settingsSetupLoader);

Q_SIGNALS:
	void settingsSetupLoaderChanged(QtMvvm::ISettingsSetupLoader* settingsSetupLoader);

protected:
	void onInit(const QVariantHash &params) override;

private:
	QScopedPointer<SettingsViewModelPrivate> d;
};

}

#endif // QTMVVM_SETTINGSVIEWMODEL_H

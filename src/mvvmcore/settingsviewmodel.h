#ifndef QTMVVM_SETTINGSVIEWMODEL_H
#define QTMVVM_SETTINGSVIEWMODEL_H

#include <QtCore/qsettings.h>
#include <QtCore/qscopedpointer.h>

#include <QtMvvmCore/viewmodel.h>
#include <QtMvvmCore/message.h>

#include "QtMvvmCore/qtmvvmcore_global.h"
#include "QtMvvmCore/settingssetup.h"

namespace QtMvvm {

class SettingsViewModelPrivate;
//! A ViewModel for a generic settings dialog based of an XML settings file
class Q_MVVMCORE_EXPORT SettingsViewModel : public ViewModel
{
	Q_OBJECT

	//! Specifies if restoring the defaults is generally allowed
	Q_PROPERTY(bool canRestoreDefaults READ canRestoreDefaults CONSTANT)
	//! The message configuration to be used to for a dialog to ask for settings restore
	Q_PROPERTY(QtMvvm::MessageConfig restoreConfig READ restoreConfig CONSTANT)

	//! The settings setup loader to use to create the settings dialog. Is an injected property
	Q_PROPERTY(QtMvvm::ISettingsSetupLoader* settingsSetupLoader READ settingsSetupLoader WRITE setSettingsSetupLoader NOTIFY settingsSetupLoaderChanged)
	QTMVVM_INJECT(QtMvvm::ISettingsSetupLoader*, settingsSetupLoader)

public:
	//! The parameter for a QSettings object for the onInit() method
	static const QString paramSettings;
	//! The parameter for a settings setup file for the onInit() method
	static const QString paramSetupFile;

	//! Generates show parameter to show a settings viewmodel via ViewModel::show
	static QVariantHash showParams(QSettings *settings, const QString &setupFile = {});

	//! Invokable constructor
	Q_INVOKABLE explicit SettingsViewModel(QObject *parent = nullptr);
	~SettingsViewModel() override;

	//! @readAcFn{SettingsViewModel::canRestoreDefaults}
	virtual bool canRestoreDefaults() const;
	//! @readAcFn{SettingsViewModel::restoreConfig}
	virtual QtMvvm::MessageConfig restoreConfig() const;
	//! @readAcFn{SettingsViewModel::settingsSetupLoader}
	ISettingsSetupLoader* settingsSetupLoader() const;

	//! Loads the settings setup of the prepared file for the given frontend
	SettingsElements::Setup loadSetup(const QString &frontend) const;

	//! Returns the settings this viewmodel operates on
	QSettings *settings() const;

	//! Loads the value for the given key from the settings
	Q_INVOKABLE virtual QVariant loadValue(const QString &key, const QVariant &defaultValue = {}) const;
	//! Saves the value with the given key
	Q_INVOKABLE virtual void saveValue(const QString &key, const QVariant &value);
	//! Resets the value or group identified by the key
	Q_INVOKABLE virtual void resetValue(const QString &key);

public Q_SLOTS:
	//! Is called when an action type edit is pressed
	virtual void callAction(const QString &key, const QVariantMap &parameters);

	//! @writeAcFn{SettingsViewModel::settingsSetupLoader}
	void setSettingsSetupLoader(QtMvvm::ISettingsSetupLoader* settingsSetupLoader);

Q_SIGNALS:
	//! @notifyAcFn{SettingsViewModel::settingsSetupLoader}
	void settingsSetupLoaderChanged(QtMvvm::ISettingsSetupLoader* settingsSetupLoader, QPrivateSignal);

	//! Is emitted when the initialization has been completed and the viewmodel is ready for loading settings
	void beginLoadSetup();

	Q_REVISION(1) void valueChanged(const QString &key); //TODO add to save/reset doc

protected:
	void onInit(const QVariantHash &params) override;

private:
	QScopedPointer<SettingsViewModelPrivate> d;
};

}

#endif // QTMVVM_SETTINGSVIEWMODEL_H

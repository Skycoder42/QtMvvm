#ifndef QTMVVM_SETTINGSVIEWMODEL_H
#define QTMVVM_SETTINGSVIEWMODEL_H

#include <QtCore/qsettings.h>
#include <QtCore/qscopedpointer.h>

#include <QtMvvmCore/viewmodel.h>
#include <QtMvvmCore/message.h>

#include "QtMvvmCore/qtmvvmcore_global.h"
#include "QtMvvmCore/settingssetup.h"
#include "QtMvvmCore/isettingsaccessor.h"

namespace QtMvvm {

class SettingsViewModelPrivate;
//! A ViewModel for a generic settings dialog based of an XML settings file
class Q_MVVMCORE_EXPORT SettingsViewModel : public ViewModel
{
	Q_OBJECT

	//! Holds the settings accessor used by the viewmodel to access the settings
	Q_PROPERTY(QtMvvm::ISettingsAccessor* accessor READ accessor WRITE setAccessor NOTIFY accessorChanged REVISION 1)

	//! Specifies if restoring the defaults is generally allowed
	Q_PROPERTY(bool canRestoreDefaults READ canRestoreDefaults CONSTANT)
	//! The message configuration to be used to for a dialog to ask for settings restore
	Q_PROPERTY(QtMvvm::MessageConfig restoreConfig READ restoreConfig CONSTANT)

	//! The settings setup loader to use to create the settings dialog. Is an injected property
	Q_PROPERTY(QtMvvm::ISettingsSetupLoader* settingsSetupLoader READ settingsSetupLoader WRITE setSettingsSetupLoader NOTIFY settingsSetupLoaderChanged)
	QTMVVM_INJECT(QtMvvm::ISettingsSetupLoader*, settingsSetupLoader)

public:
	//! The parameter for an ISettingsAccessor object for the onInit() method
	static const QString paramAccessor;
	//! The parameter for a QSettings object for the onInit() method
	static const QString paramSettings;
	//! The parameter for a settings setup file for the onInit() method
	static const QString paramSetupFile;

	//! Generates show parameter to show a settings viewmodel via ViewModel::show
	static QVariantHash showParams(ISettingsAccessor *accessor, const QString &setupFile = {});
	//! @copybrief SettingsViewModel::showParams(ISettingsAccessor*, const QString &)
	static QVariantHash showParams(QSettings *settings, const QString &setupFile = {});

	//! Invokable constructor
	Q_INVOKABLE explicit SettingsViewModel(QObject *parent = nullptr);
	~SettingsViewModel() override;

	//! @readAcFn{SettingsViewModel::accessor}
	QtMvvm::ISettingsAccessor* accessor() const;
	//! @readAcFn{SettingsViewModel::canRestoreDefaults}
	virtual bool canRestoreDefaults() const;
	//! @readAcFn{SettingsViewModel::restoreConfig}
	virtual QtMvvm::MessageConfig restoreConfig() const;
	//! @readAcFn{SettingsViewModel::settingsSetupLoader}
	ISettingsSetupLoader* settingsSetupLoader() const;

	//! Loads the settings setup of the prepared file for the given frontend
	SettingsElements::Setup loadSetup(const QString &frontend) const;

	//! Returns the settings this viewmodel operates on (or null if not using QtMvvm::QSettingsAccessor)
	QSettings *settings() const;

	//! Loads the value for the given key from the settings
	Q_INVOKABLE virtual QVariant loadValue(const QString &key, const QVariant &defaultValue = {}) const;
	//! Saves the value with the given key
	Q_INVOKABLE virtual void saveValue(const QString &key, const QVariant &value);
	//! Resets the value or group identified by the key
	Q_INVOKABLE virtual void resetValue(const QString &key);
	//! Resets all values that are defined by the entries in the given setup
	QTMVVM_REVISION_1 Q_INVOKABLE void resetAll(const SettingsElements::Setup &setup);

public Q_SLOTS:
	//! Is called when an action type edit is pressed
	virtual void callAction(const QString &key, const QVariantMap &parameters);

	//! @writeAcFn{SettingsViewModel::accessor}
	QTMVVM_REVISION_1 void setAccessor(QtMvvm::ISettingsAccessor* accessor);
	//! @writeAcFn{SettingsViewModel::settingsSetupLoader}
	void setSettingsSetupLoader(QtMvvm::ISettingsSetupLoader* settingsSetupLoader);

Q_SIGNALS:
	//! @notifyAcFn{SettingsViewModel::accessor}
	QTMVVM_REVISION_1 void accessorChanged(QtMvvm::ISettingsAccessor* accessor);
	//! @notifyAcFn{SettingsViewModel::settingsSetupLoader}
	void settingsSetupLoaderChanged(QtMvvm::ISettingsSetupLoader* settingsSetupLoader, QPrivateSignal);

	//! Is emitted when the initialization has been completed and the viewmodel is ready for loading settings
	void beginLoadSetup();

	//! Signal to be emitted whenver a value in the settings is changed or removed to update the GUI
	QTMVVM_REVISION_1 void valueChanged(const QString &key);
	//! Is emitted when the user accepted the reset triggered by resetAll()
	QTMVVM_REVISION_1 void resetAccepted(QPrivateSignal);

protected:
	void onInit(const QVariantHash &params) override;

private:
	QScopedPointer<SettingsViewModelPrivate> d;
};

}

#endif // QTMVVM_SETTINGSVIEWMODEL_H

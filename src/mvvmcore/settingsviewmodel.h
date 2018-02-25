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
class Q_MVVMCORE_EXPORT SettingsViewModel : public ViewModel
{
	Q_OBJECT

	Q_PROPERTY(bool canRestoreDefaults READ canRestoreDefaults CONSTANT)
	Q_PROPERTY(QtMvvm::MessageConfig restoreConfig READ restoreConfig CONSTANT)

	Q_PROPERTY(QtMvvm::ISettingsSetupLoader* settingsSetupLoader READ settingsSetupLoader WRITE setSettingsSetupLoader NOTIFY settingsSetupLoaderChanged)
	QTMVVM_INJECT(QtMvvm::ISettingsSetupLoader*, settingsSetupLoader)

public:
	static const QString paramSettings;
	static const QString paramSetupFile;

	static QVariantHash showParams(QSettings *settings = nullptr, const QString &setupFile = {});

	Q_INVOKABLE explicit SettingsViewModel(QObject *parent = nullptr);
	~SettingsViewModel();

	virtual bool canRestoreDefaults() const;
	virtual QtMvvm::MessageConfig restoreConfig() const;

	ISettingsSetupLoader* settingsSetupLoader() const;
	SettingsElements::Setup loadSetup(const QString &frontend) const;

	QSettings *settings() const;

	Q_INVOKABLE virtual QVariant loadValue(const QString &key, const QVariant &defaultValue = {}) const;
	Q_INVOKABLE virtual void saveValue(const QString &key, const QVariant &value);
	Q_INVOKABLE virtual void resetValue(const QString &key);

public Q_SLOTS:
	virtual void callAction(const QString &entryId, const QVariantMap &parameters);

	void setSettingsSetupLoader(QtMvvm::ISettingsSetupLoader* settingsSetupLoader);

Q_SIGNALS:
	void settingsSetupLoaderChanged(QtMvvm::ISettingsSetupLoader* settingsSetupLoader);

	void beginLoadSetup();

protected:
	void onInit(const QVariantHash &params) override;

private:
	QScopedPointer<SettingsViewModelPrivate> d;
};

}

#endif // QTMVVM_SETTINGSVIEWMODEL_H

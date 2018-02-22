#include "settingsviewmodel.h"
#include "settingsviewmodel_p.h"
#include <QtMvvmCore/CoreApp>
#include <QtMvvmCore/private/qtmvvm_logging_p.h>

#include "settingssetuploader_p.h"

using namespace QtMvvm;

SettingsViewModel::SettingsViewModel(QObject *parent) :
	ViewModel(parent),
	d(new SettingsViewModelPrivate())
{}

SettingsViewModel::~SettingsViewModel() {}

void SettingsViewModel::showSettings(ViewModel *parent)
{
	showSettings(nullptr, {}, parent);
}

void SettingsViewModel::showSettings(QSettings *settings, const QString &setupFile, ViewModel *parent)
{

}

bool SettingsViewModel::canRestoreDefaults() const
{
	return true;
}

ISettingsSetupLoader *SettingsViewModel::settingsSetupLoader() const
{
	return d->settingsSetupLoader;
}

void SettingsViewModel::callAction(const QString &entryId)
{
	logWarning() << "Unknown action requested with entry id:" << entryId;
}

void SettingsViewModel::setSettingsSetupLoader(ISettingsSetupLoader *settingsSetupLoader)
{
	if (d->settingsSetupLoader == settingsSetupLoader)
		return;

	d->settingsSetupLoader = settingsSetupLoader;
	emit settingsSetupLoaderChanged(d->settingsSetupLoader);
}

void SettingsViewModel::onInit(const QVariantHash &params)
{

}

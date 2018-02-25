#include "settingsviewmodel.h"
#include "settingsviewmodel_p.h"
#include "coreapp.h"
#include "qtmvvm_logging_p.h"
#include "settingssetuploader_p.h"

using namespace QtMvvm;

const QString SettingsViewModel::paramSettings(QStringLiteral("settings"));
const QString SettingsViewModel::paramSetupFile(QStringLiteral("setupFile"));

SettingsViewModel::SettingsViewModel(QObject *parent) :
	ViewModel(parent),
	d(new SettingsViewModelPrivate())
{}

SettingsViewModel::~SettingsViewModel() {}

QVariantHash SettingsViewModel::showParams(QSettings *settings, const QString &setupFile)
{
	return {
		{paramSettings, QVariant::fromValue(settings)},
		{paramSetupFile, setupFile}
	};
}

bool SettingsViewModel::canRestoreDefaults() const
{
	return true;
}

MessageConfig SettingsViewModel::restoreConfig() const
{
	MessageConfig config;
	config.setType(MessageConfig::TypeMessageBox);
	config.setSubType(MessageConfig::SubTypeWarning);
	config.setTitle(tr("Restore Defaults?"));
	config.setText(tr("All custom changes will be deleted and the defaults restored. <i>This cannot be undone!</i>"));
	config.setButtons(MessageConfig::Yes | MessageConfig::No);
	return config;
}

ISettingsSetupLoader *SettingsViewModel::settingsSetupLoader() const
{
	return d->setupLoader;
}

SettingsElements::Setup SettingsViewModel::loadSetup(const QString &frontend) const
{
	try {
		QFileSelector selector;
		return d->setupLoader->loadSetup(d->setupFile, frontend, &selector);
	} catch(SettingsLoaderException &e) {
		logCritical() << "Failed to load settings setup:" << e.what();
		return {};
	}
}

QSettings *SettingsViewModel::settings() const
{
	return d->settings;
}

QVariant SettingsViewModel::loadValue(const QString &key, const QVariant &defaultValue) const
{
	return d->settings->value(key, defaultValue);
}

void SettingsViewModel::saveValue(const QString &key, const QVariant &value)
{
	d->settings->setValue(key, value);
}

void SettingsViewModel::resetValue(const QString &key)
{
	d->settings->remove(key);
}

void SettingsViewModel::callAction(const QString &entryId, const QVariantMap &parameters)
{
	Q_UNUSED(parameters)
	logWarning() << "Unknown action requested with entry id" << entryId
				 << "and parameters" << parameters;
}

void SettingsViewModel::setSettingsSetupLoader(ISettingsSetupLoader *settingsSetupLoader)
{
	if (d->setupLoader == settingsSetupLoader)
		return;

	d->setupLoader = settingsSetupLoader;
	emit settingsSetupLoaderChanged(d->setupLoader);
}

void SettingsViewModel::onInit(const QVariantHash &params)
{
	Q_ASSERT_X(d->setupLoader, Q_FUNC_INFO, "settingsSetupLoader must not be null");

	d->settings = params.value(paramSettings).value<QSettings*>();
	if(!d->settings)
		d->settings = new QSettings(this);
	d->setupFile = params.value(paramSetupFile).toString();
	if(d->setupFile.isEmpty())
		d->setupFile = QStringLiteral(":/etc/settings.xml");

	emit beginLoadSetup();
}

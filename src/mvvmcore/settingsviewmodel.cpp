#include "settingsviewmodel.h"
#include "settingsviewmodel_p.h"
#include "coreapp.h"
#include "qtmvvm_logging_p.h"
#include "qsettingsaccessor.h"

using namespace QtMvvm;

const QString SettingsViewModel::paramAccessor(QStringLiteral("accessor"));
const QString SettingsViewModel::paramSettings(QStringLiteral("settings"));
const QString SettingsViewModel::paramSetupFile(QStringLiteral("setupFile"));

SettingsViewModel::SettingsViewModel(QObject *parent) :
	ViewModel(parent),
	d(new SettingsViewModelPrivate())
{}

ISettingsAccessor *SettingsViewModel::accessor() const
{
	return d->accessor;
}

SettingsViewModel::~SettingsViewModel() = default;

QVariantHash SettingsViewModel::showParams(ISettingsAccessor *accessor, const QString &setupFile)
{
	return {
		{paramAccessor, QVariant::fromValue(accessor)},
		{paramSetupFile, setupFile}
	};
}

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
	MessageConfig config {MessageConfig::TypeMessageBox, MessageConfig::SubTypeWarning};
	config.setTitle(tr("Restore Defaults?"))
			.setText(tr("All custom changes will be deleted and the defaults restored. <i>This cannot be undone!</i>"))
			.setButtons(MessageConfig::Yes | MessageConfig::No);
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
	auto qAccessor = qobject_cast<QSettingsAccessor*>(d->accessor);
	return qAccessor ? qAccessor->settings() : nullptr;
}

QVariant SettingsViewModel::loadValue(const QString &key, const QVariant &defaultValue) const
{
	return d->accessor->load(key, defaultValue);
}

void SettingsViewModel::saveValue(const QString &key, const QVariant &value)
{
	d->accessor->save(key, value);
}

void SettingsViewModel::resetValue(const QString &key)
{
	d->accessor->remove(key);
}

void SettingsViewModel::resetAll(const SettingsElements::Setup &setup)
{
	if(!canRestoreDefaults())
		return;

	auto result = CoreApp::showDialog(restoreConfig());
	connect(result, &MessageResult::dialogDone, this, [this, setup](MessageConfig::StandardButton btn) {
		if(btn != MessageConfig::Yes)
			return;
		for(const auto &category : setup.categories) {
			for(const auto &section : category.sections) {
				for(const auto &group : section.groups) {
					for(const auto &entry : group.entries) {
						resetValue(entry.key);
					}
				}
			}
		}
		emit resetAccepted({});
	}, Qt::QueuedConnection);
}

void SettingsViewModel::callAction(const QString &key, const QVariantMap &parameters)
{
	Q_UNUSED(parameters)
	logWarning() << "Unknown action requested with entry id" << key
				 << "and parameters" << parameters;
}

void QtMvvm::SettingsViewModel::setAccessor(ISettingsAccessor *accessor)
{
	if(accessor == d->accessor)
		return;

	if(d->accessor) {
		disconnect(d->accessor, &ISettingsAccessor::entryChanged,
				   this, &SettingsViewModel::valueChanged);
		disconnect(d->accessor, &ISettingsAccessor::entryRemoved,
				   this, &SettingsViewModel::valueChanged);
		if(d->accessor->parent() == this)
			d->accessor->deleteLater();
	}
	d->accessor = accessor;

	if(d->accessor) {
		connect(d->accessor, &ISettingsAccessor::entryChanged,
				this, &SettingsViewModel::valueChanged);
		connect(d->accessor, &ISettingsAccessor::entryRemoved,
				this, &SettingsViewModel::valueChanged);
	}

	emit accessorChanged(d->accessor);
}

void SettingsViewModel::setSettingsSetupLoader(ISettingsSetupLoader *settingsSetupLoader)
{
	if (d->setupLoader == settingsSetupLoader)
		return;

	d->setupLoader = settingsSetupLoader;
	emit settingsSetupLoaderChanged(d->setupLoader, {});
}

void SettingsViewModel::onInit(const QVariantHash &params)
{
	Q_ASSERT_X(d->setupLoader, Q_FUNC_INFO, "settingsSetupLoader must not be null");

	auto accessor = params.value(paramAccessor).value<ISettingsAccessor*>();
	if(!accessor) {
		auto settings = params.value(paramSettings).value<QSettings*>();
		if(settings)
			accessor = new QSettingsAccessor{settings, this};
		else
			accessor = new QSettingsAccessor{this};
	}
	setAccessor(accessor);

	d->setupFile = params.value(paramSetupFile).toString();
	if(d->setupFile.isEmpty())
		d->setupFile = QStringLiteral(":/etc/settings.xml");

	emit beginLoadSetup();
}

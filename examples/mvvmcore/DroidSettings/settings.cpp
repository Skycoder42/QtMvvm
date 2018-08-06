#include "settings.h"

Settings::Settings(QObject *parent) :
	QObject{parent},
	_accessor{new QtMvvm::AndroidSettingsAccessor{}}
{
	connect(_accessor, &QtMvvm::AndroidSettingsAccessor::entryChanged,
			this, &Settings::entryChanged);
	connect(_accessor, &QtMvvm::AndroidSettingsAccessor::entryRemoved,
			this, &Settings::entryRemoved);
}

QString Settings::load(const QString &key)
{
	return _accessor->load(key, tr("<unset>")).toString();
}

void Settings::save(const QString &key, const QString &value)
{
	_accessor->save(key, value);
}

void Settings::remove(const QString &key)
{
	_accessor->remove(key);
}

void Settings::entryChanged(const QString &key, const QVariant &value)
{
	emit changeEvent(tr("Data for key <%1> changed to: %2")
					 .arg(key, value.toString()));
}

void Settings::entryRemoved(const QString &key)
{
	emit changeEvent(tr("Data for key <%1> removed").arg(key));
}

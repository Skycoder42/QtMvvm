#include "testbackend.h"

TestBackend::TestBackend(QObject *parent) :
	ISettingsAccessor{parent}
{}

bool TestBackend::contains(const QString &key) const
{
	return _data.contains(key);
}

QVariant TestBackend::load(const QString &key, const QVariant &defaultValue) const
{
	return _data.value(key, defaultValue);
}

void TestBackend::save(const QString &key, const QVariant &value)
{
	_data.insert(key, value);
	emit entryChanged(key, value);
}

void TestBackend::remove(const QString &key)
{
	for(auto it = _data.begin(); it != _data.end();) {
		if(it.key().startsWith(key + QLatin1Char('/'))) {
			it = _data.erase(it);
			emit entryRemoved(key);
		} else
			++it;
	}
	_data.remove(key);
	emit entryRemoved(key);
}

void TestBackend::sync()
{
}

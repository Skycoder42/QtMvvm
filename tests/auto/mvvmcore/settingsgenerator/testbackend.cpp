#include "testbackend.h"
#include <QDebug>

TestBackend::TestBackend(QString name, int code, QObject *parent) :
	ISettingsAccessor{parent},
	_name{std::move(name)},
	_code{code}
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
			auto rmKey = it.key();
			it = _data.erase(it);
			emit entryRemoved(rmKey);
		} else
			++it;
	}
	_data.remove(key);
	emit entryRemoved(key);
}

void TestBackend::sync()
{
}

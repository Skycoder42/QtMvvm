#include "testbackend.h"

TestBackend::TestBackend(QString name, int code, QObject *parent) :
	ISettingsAccessor{parent},
	_name{std::move(name)},
	_code{code}
{}

bool TestBackend::contains(const QString &key) const
{
	return false;
}

QVariant TestBackend::load(const QString &key, const QVariant &defaultValue) const
{
	return {};
}

void TestBackend::save(const QString &key, const QVariant &value)
{
}

void TestBackend::remove(const QString &key)
{
}

void TestBackend::sync()
{
}

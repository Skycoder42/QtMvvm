#include "settingsentry.h"
using namespace QtMvvm;

template<>
void QtMvvm::SettingsEntry<QVariant>::set(const QVariant &value)
{
	_accessor->save(_key, value);
}

template<>
QVariant QtMvvm::SettingsEntry<QVariant>::get() const
{
	return _accessor->load(_key, _default);
}

template<>
void QtMvvm::SettingsEntry<QVariant>::addChangeCallback(QObject *scope, const std::function<void (QVariant)> &callback)
{
	auto mKey = _key;
	auto mDefault = _default;
	QObject::connect(_accessor, &ISettingsAccessor::entryChanged,
					 scope, [mKey, callback](const QString &key, const QVariant &value) {
		if(key == mKey)
			callback(value);
	});
	QObject::connect(_accessor, &ISettingsAccessor::entryRemoved,
					 scope, [mKey, mDefault, callback](const QString &key) {
		if(key == mKey)
			callback(mDefault);
	});
}

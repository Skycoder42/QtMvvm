#ifndef QTMVVM_SETTINGSENTRY_H
#define QTMVVM_SETTINGSENTRY_H

#include "QtMvvmCore/isettingsaccessor.h"

namespace QtMvvm {

template <typename T>
class SettingsEntry
{
	Q_DISABLE_COPY(SettingsEntry)

public:
	SettingsEntry() = default;

	bool isSet() const;

	T get() const;
	void set(const T &value);
	void reset();

	SettingsEntry<T> &operator=(const T &value);
	operator const T() const;

	void addChangeCallback(const std::function<void(T)> &callback);
	void addChangeCallback(QObject *scope, const std::function<void(T)> &callback);

	// internal
	void setup(const QString &key, ISettingsAccessor *accessor, const QVariant &defaultValue = {});

private:
	QString _key;
	ISettingsAccessor *_accessor = nullptr;
	QVariant _default;
};

template <>
class SettingsEntry<void>
{
	Q_DISABLE_COPY(SettingsEntry)

public:
	SettingsEntry() = default;

	// internal
	void setup(const QString &, ISettingsAccessor *, const QVariant & = {}) {}
};


// ------------- Generic Implementation -------------

template<typename T>
bool SettingsEntry<T>::isSet() const
{
	return _accessor->contains(_key);
}

template<typename T>
T SettingsEntry<T>::get() const
{
	return _accessor->load(_key, _default).template value<T>();
}

template<typename T>
void SettingsEntry<T>::set(const T &value)
{
	_accessor->save(_key, QVariant::fromValue(value));
}

template<typename T>
void SettingsEntry<T>::reset()
{
	_accessor->remove(_key);
}

template<typename T>
SettingsEntry<T> &SettingsEntry<T>::operator=(const T &value)
{
	set(value);
	return (*this);
}

template<typename T>
void SettingsEntry<T>::addChangeCallback(const std::function<void (T)> &callback)
{
	addChangeCallback(_accessor, callback);
}

template<typename T>
void SettingsEntry<T>::addChangeCallback(QObject *scope, const std::function<void (T)> &callback)
{
	auto mKey = _key;
	auto mDefault = _default;
	QObject::connect(_accessor, &ISettingsAccessor::entryChanged,
					 scope, [mKey, callback](const QString &key, const QVariant &value) {
		if(key == mKey)
			callback(value.template value<T>());
	});
	QObject::connect(_accessor, &ISettingsAccessor::entryRemoved,
					 scope, [mKey, mDefault, callback](const QString &key) {
		if(key == mKey)
			callback(mDefault.template value<T>());
	});
}

template<typename T>
SettingsEntry<T>::operator const T() const
{
	return get();
}

template<typename T>
void SettingsEntry<T>::setup(const QString &key, ISettingsAccessor *accessor, const QVariant &defaultValue)
{
	Q_ASSERT_X(accessor, Q_FUNC_INFO, "You must set a valid accessor before initializing the settings!");
	_key  = key;
	_accessor = accessor;
	_default = defaultValue;
}

}


#endif // QTMVVM_SETTINGSENTRY_H

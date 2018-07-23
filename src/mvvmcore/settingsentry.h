#ifndef QTMVVM_SETTINGSENTRY_H
#define QTMVVM_SETTINGSENTRY_H

#include <QtCore/qlist.h>
#include <QtCore/qregularexpression.h>

#include "QtMvvmCore/qtmvvmcore_global.h"
#include "QtMvvmCore/isettingsaccessor.h"

namespace QtMvvm {

template <typename T>
class SettingsEntry
{
	Q_DISABLE_COPY(SettingsEntry)

public:
	SettingsEntry() = default;

	bool isSet() const;
	QString key() const;

	T get() const;
	void set(const T &value);
	void reset();

	SettingsEntry<T> &operator=(const T &value);
	operator T() const;

	void addChangeCallback(const std::function<void(T)> &callback);
	void addChangeCallback(QObject *scope, const std::function<void(T)> &callback);

	// internal
	void setup(QString key, ISettingsAccessor *accessor, QVariant defaultValue = {});

private:
	QString _key;
	ISettingsAccessor *_accessor = nullptr;
	QVariant _default;
};

template <typename TType>
class SettingsEntry<QList<TType>>
{
	Q_DISABLE_COPY(SettingsEntry)

public:
	class ListElement
	{
		Q_DISABLE_COPY(ListElement)

	public:
		inline ListElement(ListElement &&other) noexcept = default;
		inline ListElement &operator=(ListElement &&other) noexcept = default;

		TType get() const && { return _self->getAt(_index); }
		void set(const TType &value) && { _self->setAt(_index, value); }

		inline ListElement &operator=(const TType &value) && { _self->setAt(_index, value); return *this; }
		inline operator TType() const && { return _self->getAt(_index); }

	private:
		friend class SettingsEntry<QList<TType>>;
		inline ListElement(SettingsEntry<QList<TType>> *self, int index) : _self{self}, _index{index} {}

		SettingsEntry<QList<TType>> *_self;
		int _index;
	};

	template <typename T>
	class iterator_base;

	class iterator_value
	{
	public:
		TType get() const { return _self->getAt(_index); }
		void set(const TType &value) { _self->setAt(_index, value); }

		inline iterator_value &operator=(const TType &value) { _self->setAt(_index, value); return *this; }
		inline operator TType() const { return _self->getAt(_index); }

		inline friend void swap(iterator_value& lhs, iterator_value& rhs) { qSwap(lhs._self, rhs._self); qSwap(lhs._index, rhs._index); }

		inline friend bool operator==(const iterator_value &lhs, const iterator_value &rhs) { assert(lhs._self == rhs._self); return lhs._index == rhs._index; }
		inline friend bool operator!=(const iterator_value &lhs, const iterator_value &rhs) { assert(lhs._self == rhs._self); return lhs._index != rhs._index; }
		inline friend bool operator<(const iterator_value &lhs, const iterator_value &rhs) { assert(lhs._self == rhs._self); return lhs._index < rhs._index; }
		inline friend bool operator>(const iterator_value &lhs, const iterator_value &rhs) { assert(lhs._self == rhs._self); return lhs._index > rhs._index; }
		inline friend bool operator<=(const iterator_value &lhs, const iterator_value &rhs) { assert(lhs._self == rhs._self); return lhs._index <= rhs._index; }
		inline friend bool operator>=(const iterator_value &lhs, const iterator_value &rhs) { assert(lhs._self == rhs._self); return lhs._index >= rhs._index; }

		inline friend iterator_value operator+(const iterator_value &value, int delta) { return iterator_value{value._self, value._index + delta}; }
		inline friend iterator_value operator+(int delta, const iterator_value &value) { return iterator_value{value._self, value._index + delta}; }
		inline friend iterator_value operator-(const iterator_value &value, int delta) { return iterator_value{value._self, value._index - delta}; }
		inline friend int operator-(const iterator_value &lhs, const iterator_value &rhs) { assert(lhs._self == rhs._self); return lhs._index - rhs._index; }

	private:
		template <typename T>
		friend class SettingsEntry<QList<TType>>::iterator_base;

		inline iterator_value(SettingsEntry<QList<TType>> *self, int index) : _self{self}, _index{index} {}
		inline iterator_value(const iterator_value &other) = default;
		inline iterator_value &operator=(const iterator_value &other) = default;
		inline iterator_value(iterator_value &&other) noexcept = default;
		inline iterator_value &operator=(iterator_value &&other) noexcept = default;

		SettingsEntry<QList<TType>> *_self;
		int _index;
	};

	template <typename T>
	class iterator_base
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		using difference_type = int;
		using value_type = T;
		using pointer = value_type*;
		using reference = value_type&;

		inline iterator_base(const iterator_base<T> &other) = default;
		inline iterator_base &operator=(const iterator_base<T> &other) = default;
		inline iterator_base(iterator_base<T> &&other) noexcept = default;
		inline iterator_base &operator=(iterator_base<T> &&other) noexcept = default;

		inline iterator_base<T> &operator++() { ++_value._index; return *this; }
		inline iterator_base<T> operator++(int) { return iterator_base<T>{_value._self, _value._index++}; }
		inline iterator_base<T> &operator--() { --_value._index; return *this; }
		inline iterator_base<T> operator--(int) { return iterator_base<T>{_value._self, _value._index--}; }

		inline const value_type &operator*() const { return _value; }
		inline value_type &operator*() { return _value; }
		inline const value_type *operator->() const { return &_value; }
		inline value_type *operator->() { return &_value; }
		inline friend void swap(iterator_base<T> &lhs, iterator_base<T> &rhs) { swap(lhs._value, rhs._value); }

		inline friend bool operator==(const iterator_base<T> &lhs, const iterator_base<T> &rhs) { return lhs._value == rhs._value; }
		inline friend bool operator!=(const iterator_base<T> &lhs, const iterator_base<T> &rhs) { return lhs._value != rhs._value; }
		inline friend bool operator<(const iterator_base<T> &lhs, const iterator_base<T> &rhs) { return lhs._value < rhs._value; }
		inline friend bool operator>(const iterator_base<T> &lhs, const iterator_base<T> &rhs) { return lhs._value > rhs._value; }
		inline friend bool operator<=(const iterator_base<T> &lhs, const iterator_base<T> &rhs) { return lhs._value <= rhs._value; }
		inline friend bool operator>=(const iterator_base<T> &lhs, const iterator_base<T> &rhs) { return lhs._value >= rhs._value; }

		inline iterator_base<T> &operator+=(difference_type delta) { _value._index += delta; return *this; }
		inline friend iterator_base<T> operator+(const iterator_base<T> &iter, difference_type delta) { return iterator_base<T>{iter._value + delta}; }
		inline friend iterator_base<T> operator+(difference_type delta, const iterator_base<T> &iter) { return iterator_base<T>{iter._value + delta}; }
		inline iterator_base<T> &operator-=(difference_type delta) { _value._index -= delta; return *this; }
		inline friend iterator_base<T> operator-(const iterator_base<T> &iter, difference_type delta) { return iterator_base<T>{iter._value - delta}; }
		inline friend difference_type operator-(const iterator_base<T> &lhs, const iterator_base<T> &rhs) { return lhs._value - rhs._value; }

		inline value_type operator[](difference_type delta) const { return iterator_value{_value._self, _value._index + delta}; }

	private:
		friend class SettingsEntry<QList<TType>>;

		inline iterator_base(SettingsEntry<QList<TType>> *self, int index) : _value{self, index} {}
		inline iterator_base(iterator_value value) : _value{std::move(value)} {}
		iterator_value _value;
	};

	using iterator = iterator_base<iterator_value>;
	using const_iterator = iterator_base<const iterator_value>;

	SettingsEntry() = default;

	bool isSet() const;
	QString key() const;

	QList<TType> get() const;
	void set(const QList<TType> &value);
	void reset(bool reInit = true);

	SettingsEntry<QList<TType>> &operator=(const QList<TType> &value);
	operator QList<TType>() const;

	int size() const;
	TType getAt(int index) const;
	void setAt(int index, const TType &value);
	void push(const TType &value);
	void push(const QList<TType> &values);
	TType pop();
	void chop(int count);

	ListElement operator[](int index);
	const ListElement operator[](int index) const;
	SettingsEntry<QList<TType>> &operator+=(const TType &value);
	SettingsEntry<QList<TType>> &operator+=(const QList<TType> &values);

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator constBegin() const;
	const_iterator constEnd() const;

	void addChangeCallback(const std::function<void(QList<TType>)> &callback);
	void addChangeCallback(QObject *scope, const std::function<void(QList<TType>)> &callback);
	void addChangeCallback(const std::function<void(int, TType)> &callback); // index, value
	void addChangeCallback(QObject *scope, const std::function<void(int, TType)> &callback);
	void addSizeChangeCallback(const std::function<void(int)> &callback); // size
	void addSizeChangeCallback(QObject *scope, const std::function<void(int)> &callback);

	// internal
	void setup(QString key, ISettingsAccessor *accessor, QVariant defaultValue = {});
	void setupInit(QList<TType> init);

private:
	QString _key;
	ISettingsAccessor *_accessor = nullptr;
	QVariant _default;
	QList<TType> _init;
};

template <>
class SettingsEntry<void>
{
	Q_DISABLE_COPY(SettingsEntry)

public:
	inline SettingsEntry() = default;

	// internal
	inline void setup(const QString &, ISettingsAccessor *, const QVariant & = {}) {}
};

// ------------- Generic Implementation -------------

template<typename T>
bool SettingsEntry<T>::isSet() const
{
	return _accessor->contains(_key);
}

template<typename T>
QString SettingsEntry<T>::key() const
{
	return _key;
}

template<typename T>
T SettingsEntry<T>::get() const
{
	return _accessor->load(_key, _default).template value<T>();
}

template<>
Q_MVVMCORE_EXPORT QVariant SettingsEntry<QVariant>::get() const;

template<typename T>
void SettingsEntry<T>::set(const T &value)
{
	_accessor->save(_key, QVariant::fromValue(value));
}

template<>
Q_MVVMCORE_EXPORT void SettingsEntry<QVariant>::set(const QVariant &value);

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
SettingsEntry<T>::operator T() const
{
	return get();
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

template<>
Q_MVVMCORE_EXPORT void SettingsEntry<QVariant>::addChangeCallback(QObject *scope, const std::function<void (QVariant)> &callback);

template<typename T>
void SettingsEntry<T>::setup(QString key, ISettingsAccessor *accessor, QVariant defaultValue)
{
	Q_ASSERT_X(accessor, Q_FUNC_INFO, "You must set a valid accessor before initializing the settings!");
	_key = std::move(key);
	_accessor = accessor;
	_default = std::move(defaultValue);
}


// ------------- Generic Implementation ListEntry -------------

template<typename TType>
bool SettingsEntry<QList<TType>>::isSet() const
{
	return _accessor->contains(_key + QStringLiteral("/size"));
}

template<typename TType>
QString SettingsEntry<QList<TType>>::key() const
{
	return _key;
}

template<typename TType>
QList<TType> SettingsEntry<QList<TType>>::get() const
{
	auto mSize = size();
	QList<TType> resList;
	resList.reserve(mSize);
	for(auto i = 0; i < mSize; ++i)
		resList.append(getAt(i));
	return resList;
}

template<typename TType>
void SettingsEntry<QList<TType>>::set(const QList<TType> &value)
{
	reset(false);
	push(value);
}

template<typename TType>
void SettingsEntry<QList<TType>>::reset(bool reInit)
{
	_accessor->remove(_key);
	if(reInit && !_init.isEmpty())
		push(_init);
}

template<typename TType>
SettingsEntry<QList<TType>> &SettingsEntry<QList<TType>>::operator=(const QList<TType> &value)
{
	set(value);
	return *this;
}

template<typename TType>
SettingsEntry<QList<TType>>::operator QList<TType>() const
{
	return get();
}

template<typename TType>
int SettingsEntry<QList<TType>>::size() const
{
	return _accessor->load(_key + QStringLiteral("/size"), 0).toInt();
}

template<typename TType>
TType SettingsEntry<QList<TType>>::getAt(int index) const
{
	return _accessor->load(_key + QStringLiteral("/%1/value").arg(index), _default).template value<TType>();
}

template<typename TType>
void SettingsEntry<QList<TType>>::setAt(int index, const TType &value)
{
	_accessor->save(_key + QStringLiteral("/%1/value").arg(index), QVariant::fromValue(value));
}

template<typename TType>
void SettingsEntry<QList<TType>>::push(const TType &value)
{
	push(QList<TType>{value});
}

template<typename TType>
void SettingsEntry<QList<TType>>::push(const QList<TType> &values)
{
	auto cIndex = size();
	for(const auto &value : values)
		setAt(cIndex++, value);
	_accessor->save(_key + QStringLiteral("/size"), cIndex);
}

template<typename TType>
TType QtMvvm::SettingsEntry<QList<TType>>::pop()
{
	auto res = getAt(size() - 1);
	chop(1);
	return res;
}

template<typename TType>
void QtMvvm::SettingsEntry<QList<TType>>::chop(int count)
{
	auto cSize = size();
	auto nSize = qMax(size() - count, 0);
	for(auto i = cSize - 1; i >= nSize; --i)
		_accessor->remove(_key + QStringLiteral("/%1/value").arg(i));
	_accessor->save(_key + QStringLiteral("/size"), nSize);
}

template<typename TType>
const typename SettingsEntry<QList<TType>>::ListElement SettingsEntry<QList<TType>>::operator[](int index) const
{
	return ListElement{this, index};
}


template<typename TType>
typename SettingsEntry<QList<TType>>::ListElement SettingsEntry<QList<TType>>::operator[](int index)
{
	return ListElement{this, index};
}

template<typename TType>
SettingsEntry<QList<TType>> &SettingsEntry<QList<TType>>::operator+=(const TType &value)
{
	push(value);
	return *this;
}

template<typename TType>
SettingsEntry<QList<TType>> &SettingsEntry<QList<TType>>::operator+=(const QList<TType> &values)
{
	push(values);
	return *this;
}

template<typename TType>
typename SettingsEntry<QList<TType>>::iterator SettingsEntry<QList<TType>>::begin()
{
	return iterator{this, 0};
}

template<typename TType>
typename SettingsEntry<QList<TType>>::iterator QtMvvm::SettingsEntry<QList<TType>>::end()
{
	return iterator{this, size()};
}

template<typename TType>
typename SettingsEntry<QList<TType>>::const_iterator SettingsEntry<QList<TType>>::begin() const
{
	return constBegin();
}

template<typename TType>
typename SettingsEntry<QList<TType>>::const_iterator SettingsEntry<QList<TType>>::end() const
{
	return constEnd();
}

template<typename TType>
typename SettingsEntry<QList<TType>>::const_iterator SettingsEntry<QList<TType>>::constBegin() const
{
	return const_iterator{const_cast<SettingsEntry<QList<TType>>*>(this), 0};
}

template<typename TType>
typename SettingsEntry<QList<TType>>::const_iterator SettingsEntry<QList<TType>>::constEnd() const
{
	return const_iterator{const_cast<SettingsEntry<QList<TType>>*>(this), size()};
}

template<typename TType>
void SettingsEntry<QList<TType>>::addChangeCallback(const std::function<void (QList<TType>)> &callback)
{
	addChangeCallback(_accessor, callback);
}

template<typename TType>
void SettingsEntry<QList<TType>>::addChangeCallback(QObject *scope, const std::function<void (QList<TType>)> &callback)
{
	QObject::connect(_accessor, &ISettingsAccessor::entryChanged,
					 scope, [this, callback](const QString &key, const QVariant &) {
		if(key.startsWith(_key))
			callback(get());
	});
	QObject::connect(_accessor, &ISettingsAccessor::entryRemoved,
					 scope, [this, callback](const QString &key) {
		if(key.startsWith(_key))
			callback(get());
	});
}

template<typename TType>
void SettingsEntry<QList<TType>>::addChangeCallback(const std::function<void (int, TType)> &callback)
{
	addChangeCallback(_accessor, callback);
}

template<typename TType>
void SettingsEntry<QList<TType>>::addChangeCallback(QObject *scope, const std::function<void (int, TType)> &callback)
{
	QRegularExpression mKey {QStringLiteral("^%1\\/\\d+\\/value$").arg(QRegularExpression::escape(_key))};
	mKey.optimize();
	auto mDefault = _default;
	QObject::connect(_accessor, &ISettingsAccessor::entryChanged,
					 scope, [mKey, callback](const QString &key, const QVariant &value) {
		auto match = mKey.match(key);
		if(match.hasMatch())
			callback(match.captured(1).toInt(), value.template value<TType>());
	});
	QObject::connect(_accessor, &ISettingsAccessor::entryRemoved,
					 scope, [mKey, mDefault, callback](const QString &key) {
		auto match = mKey.match(key);
		if(match.hasMatch())
			callback(match.captured(1).toInt(), mDefault.template value<TType>());
	});
}

template<typename TType>
void SettingsEntry<QList<TType>>::addSizeChangeCallback(const std::function<void (int)> &callback)
{
	addSizeChangeCallback(_accessor, callback);
}

template<typename TType>
void SettingsEntry<QList<TType>>::addSizeChangeCallback(QObject *scope, const std::function<void (int)> &callback)
{
	QString mKey = _key + QStringLiteral("/size");
	auto mInit = _init;
	QObject::connect(_accessor, &ISettingsAccessor::entryChanged,
					 scope, [mKey, callback](const QString &key, const QVariant &value) {
		if(key == mKey)
			callback(value.toInt());
	});
	QObject::connect(_accessor, &ISettingsAccessor::entryRemoved,
					 scope, [mKey, mInit, callback](const QString &key) {
		if(key == mKey)
			callback(0); //is ok, as in case of a "reinit", the entry changed will be emitted next
	});
}

template<typename TType>
void SettingsEntry<QList<TType>>::setup(QString key, ISettingsAccessor *accessor, QVariant defaultValue)
{
	Q_ASSERT_X(accessor, Q_FUNC_INFO, "You must set a valid accessor before initializing the settings!");
	_key = std::move(key);
	_accessor = accessor;
	_default = std::move(defaultValue);
}

template<typename TType>
void QtMvvm::SettingsEntry<QList<TType> >::setupInit(QList<TType> init)
{
	_init = std::move(init);
	if(!isSet())
		push(_init);
}

}


#endif // QTMVVM_SETTINGSENTRY_H

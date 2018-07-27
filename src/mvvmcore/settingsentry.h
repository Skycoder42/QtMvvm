#ifndef QTMVVM_SETTINGSENTRY_H
#define QTMVVM_SETTINGSENTRY_H

#include <functional>

#include <QtCore/qlist.h>
#include <QtCore/qregularexpression.h>
#include <QtCore/qhash.h>
#include <QtCore/qsharedpointer.h>

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

template <>
class SettingsEntry<void>
{
	Q_DISABLE_COPY(SettingsEntry)

public:
	inline SettingsEntry() = default;

	// internal
	inline void setup(const QString &, ISettingsAccessor *, const QVariant & = {}) {}
};

template <typename TType>
class SettingsListNode
{
	Q_DISABLE_COPY(SettingsListNode)

public:
	class Deferred
	{
		Q_DISABLE_COPY(Deferred)

	public:
		inline Deferred(Deferred &&other) noexcept = default;
		inline Deferred &operator=(Deferred &&other) noexcept = default;
		inline ~Deferred() { commit(); }

		inline TType &element() { return _element; }

		inline TType &operator*() { return _element; }
		inline TType *operator->() { return &_element; }

		inline void commit() { if(!_commited) { _commited = true; _node->commit(_index);} }

	private:
		friend class SettingsListNode<TType>;

		SettingsListNode<TType> *_node;
		TType &_element;
		bool _commited = false;
		int _index;

		inline Deferred(SettingsListNode<TType> *node, TType &element, int index) :
			_node{node},
			_element{element},
			_index{index}
		{}
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

		inline iterator_base() = default;
		inline iterator_base(const iterator_base<T> &other) = default;
		inline iterator_base &operator=(const iterator_base<T> &other) = default;
		inline iterator_base(iterator_base<T> &&other) noexcept = default;
		inline iterator_base &operator=(iterator_base<T> &&other) noexcept = default;

		inline iterator_base<T> &operator++() { ++_index; return *this; }
		inline iterator_base<T> operator++(int) { return iterator_base<T>{_node, _index++}; }
		inline iterator_base<T> &operator--() { --_index; return *this; }
		inline iterator_base<T> operator--(int) { return iterator_base<T>{_node, _index--}; }

		inline reference operator*() const { return _node->at(_index); }
		inline pointer operator->() const { return &_node->at(_index); }
		inline friend void swap(iterator_base<T> &lhs, iterator_base<T> &rhs) noexcept { std::swap(lhs._node, rhs._node); std::swap(lhs._index, rhs._index); }

		inline friend bool operator==(const iterator_base<T> &lhs, const iterator_base<T> &rhs) { assert(lhs._node == rhs._node); return lhs._index == rhs._index; }
		inline friend bool operator!=(const iterator_base<T> &lhs, const iterator_base<T> &rhs) { assert(lhs._node == rhs._node); return lhs._index != rhs._index; }
		inline friend bool operator<(const iterator_base<T> &lhs, const iterator_base<T> &rhs) { assert(lhs._node == rhs._node); return lhs._index < rhs._index; }
		inline friend bool operator>(const iterator_base<T> &lhs, const iterator_base<T> &rhs) { assert(lhs._node == rhs._node); return lhs._index > rhs._index; }
		inline friend bool operator<=(const iterator_base<T> &lhs, const iterator_base<T> &rhs) { assert(lhs._node == rhs._node); return lhs._index <= rhs._index; }
		inline friend bool operator>=(const iterator_base<T> &lhs, const iterator_base<T> &rhs) { assert(lhs._node == rhs._node); return lhs._index >= rhs._index; }

		inline iterator_base<T> &operator+=(difference_type delta) { _index += delta; return *this; }
		inline friend iterator_base<T> operator+(const iterator_base<T> &iter, difference_type delta) { return iterator_base<T>{iter._node, iter._index + delta}; }
		inline friend iterator_base<T> operator+(difference_type delta, const iterator_base<T> &iter) { return iterator_base<T>{iter._node, iter._index + delta}; }
		inline iterator_base<T> &operator-=(difference_type delta) { _index -= delta; return *this; }
		inline friend iterator_base<T> operator-(const iterator_base<T> &iter, difference_type delta) { return iterator_base<T>{iter._node, iter._index - delta}; }
		inline friend difference_type operator-(const iterator_base<T> &lhs, const iterator_base<T> &rhs) { assert(lhs._node == rhs._node); return lhs._index - rhs._index; }

		inline reference operator[](difference_type delta) const { return _node->at(_index + delta); }

	private:
		friend class SettingsListNode<TType>;

		SettingsListNode<TType> *_node;
		int _index;

		inline iterator_base(SettingsListNode<TType> *node, int index) :
			_node{node},
			_index{index}
		{}
	};

	using iterator = iterator_base<TType>;
	using const_iterator = iterator_base<const TType>;

	SettingsListNode() = default;

	bool isSet() const;
	QString key() const;
	int size() const;
	const TType &at(int index) const;
	TType &at(int index);

	TType &push();
	Deferred push_deferred();
	void pop(int count = 1);
	void reset();

	TType &operator[](int index);
	const TType &operator[](int index) const;

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator constBegin() const;
	const_iterator constEnd() const;

	void addChangeCallback(const std::function<void(int)> &callback); // size
	void addChangeCallback(QObject *scope, const std::function<void(int)> &callback);

	// internal
	void setup(QString key, ISettingsAccessor *accessor, std::function<void(int, TType&)> setupFn);
	void commit(int index);

private:
	struct ElementHolder {
		bool __initialized = false;
		QSharedPointer<TType> _element;
		inline ElementHolder() : _element{new TType{}} {}
	};

	QString _key;
	QString _sizeKey;
	ISettingsAccessor *_accessor = nullptr;
	std::function<void(int, TType&)> _setupFn;
	mutable QHash<int, ElementHolder> _cache;
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

// ------------- Generic Implementation ListNode -------------

template<typename TType>
bool SettingsListNode<TType>::isSet() const
{
	return _accessor->contains(_sizeKey);
}

template<typename TType>
QString SettingsListNode<TType>::key() const
{
	return _key;
}

template<typename TType>
int SettingsListNode<TType>::size() const
{
	return _accessor->load(_sizeKey, 0).toInt();
}

template<typename TType>
const TType &SettingsListNode<TType>::at(int index) const
{
	auto &value = _cache[index];
	if(!value.__initialized) {
		_setupFn(index, *(value._element));
		value.__initialized = true;
	}
	return *(value._element);
}

template<typename TType>
TType &SettingsListNode<TType>::at(int index)
{
	auto &value = _cache[index];
	if(!value.__initialized) {
		_setupFn(index, *(value._element));
		value.__initialized = true;
	}
	return *(value._element);
}

template<typename TType>
TType &SettingsListNode<TType>::push()
{
	auto cIndex = size();
	_accessor->save(_sizeKey, cIndex + 1);
	return at(cIndex);
}

template<typename TType>
typename SettingsListNode<TType>::Deferred SettingsListNode<TType>::push_deferred()
{
	auto cIndex = size();
	return {this, at(cIndex), cIndex};
}

template<typename TType>
void SettingsListNode<TType>::pop(int count)
{
	auto cSize = size();
	auto nSize = qMax(size() - count, 0);
	for(auto i = cSize - 1; i >= nSize; --i)
		_accessor->remove(_key + QStringLiteral("/%1").arg(i));
	_accessor->save(_sizeKey, nSize);
}

template<typename TType>
void SettingsListNode<TType>::reset()
{
	_accessor->remove(_key);
}

template<typename TType>
const TType &SettingsListNode<TType>::operator[](int index) const
{
	return at(index);
}


template<typename TType>
TType &SettingsListNode<TType>::operator[](int index)
{
	return at(index);
}

template<typename TType>
typename SettingsListNode<TType>::iterator SettingsListNode<TType>::begin()
{
	return iterator{this, 0};
}

template<typename TType>
typename SettingsListNode<TType>::iterator SettingsListNode<TType>::end()
{
	return iterator{this, size()};
}

template<typename TType>
typename SettingsListNode<TType>::const_iterator SettingsListNode<TType>::begin() const
{
	return constBegin();
}

template<typename TType>
typename SettingsListNode<TType>::const_iterator SettingsListNode<TType>::end() const
{
	return constEnd();
}

template<typename TType>
typename SettingsListNode<TType>::const_iterator SettingsListNode<TType>::constBegin() const
{
	return const_iterator{const_cast<SettingsListNode<TType>*>(this), 0};
}

template<typename TType>
typename SettingsListNode<TType>::const_iterator SettingsListNode<TType>::constEnd() const
{
	return const_iterator{const_cast<SettingsListNode<TType>*>(this), size()};
}

template<typename TType>
void SettingsListNode<TType>::addChangeCallback(const std::function<void (int)> &callback)
{
	addChangeCallback(_accessor, callback);
}

template<typename TType>
void SettingsListNode<TType>::addChangeCallback(QObject *scope, const std::function<void (int)> &callback)
{
	QString mKey = _sizeKey;
	QObject::connect(_accessor, &ISettingsAccessor::entryChanged,
					 scope, [mKey, callback](const QString &key, const QVariant &value) {
		if(key == mKey)
			callback(value.toInt());
	});
	QObject::connect(_accessor, &ISettingsAccessor::entryRemoved,
					 scope, [mKey, callback](const QString &key) {
		if(key == mKey)
			callback(0);
	});
}

template<typename TType>
void SettingsListNode<TType>::setup(QString key, ISettingsAccessor *accessor, std::function<void(int, TType&)> setupFn)
{
	Q_ASSERT_X(accessor, Q_FUNC_INFO, "You must set a valid accessor before initializing the settings!");
	_key = std::move(key);
	_sizeKey = _key + QStringLiteral("/size");
	_accessor = accessor;
	_setupFn = std::move(setupFn);
}

template<typename TType>
void SettingsListNode<TType>::commit(int index)
{
	_accessor->save(_sizeKey, qMax(size(), index + 1));
}

}


#endif // QTMVVM_SETTINGSENTRY_H

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

//! A generic wrapper around ISettingsAccessor used by the @ref settings_generator "qsettingsgenerator"
template <typename T>
class SettingsEntry
{
	Q_DISABLE_COPY(SettingsEntry)

public:
	SettingsEntry() = default;

	//! Checks if the entry is stored in the settings
	bool isSet() const;
	//! Returns the key this entry uses to access the settings
	QString key() const;

	//! Loads the value from the settings
	T get() const;
	//! Stores the passed value in the settings
	void set(const T &value);
	//! Removes the entry and all child entries from the settings
	void reset();

	//! Stores the passed value in the settings
	SettingsEntry<T> &operator=(const T &value);
	//! Loads the value from the settings
	operator T() const;

	//! Adds a method to be called if the entries value changes
	void addChangeCallback(const std::function<void(T)> &callback);
	//! Adds a method to be called if the entries value changes, as long as scope exists
	void addChangeCallback(QObject *scope, const std::function<void(T)> &callback);

	//! @private
	void setup(QString key, ISettingsAccessor *accessor, QVariant defaultValue = {});

private:
	QString _key;
	ISettingsAccessor *_accessor = nullptr;
	QVariant _default;
};

//! @private
template <>
class SettingsEntry<void>
{
	Q_DISABLE_COPY(SettingsEntry)

public:
	inline SettingsEntry() = default;

	//! @private
	inline void setup(const QString &, ISettingsAccessor *, const QVariant & = {}) {}
};

//! A helper class used by the @ref settings_generator "qsettingsgenerator" to represent list nodes
template <typename TType>
class SettingsListNode
{
	Q_DISABLE_COPY(SettingsListNode)

public:
	//! Entry container to make deferred creation of a new list element possible
	class Deferred
	{
		Q_DISABLE_COPY(Deferred)

	public:
		//! Move constructor
		inline Deferred(Deferred &&other) noexcept = default;
		//! Move assignment operator
		inline Deferred &operator=(Deferred &&other) noexcept = default;
		inline ~Deferred() { commit(); }

		//! Returns a reference to the element beeing edited
		inline TType &element() { return _element; }

		//! Indirection operator
		inline TType &operator*() { return _element; }
		//! Member access operator
		inline TType *operator->() { return &_element; }

		//! Appends the created element to the list
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

	//! The iterator class to iterate through the list nodes entries
	template <typename T>
	class iterator_base
	{
	public:
		//! @private
		using iterator_category = std::random_access_iterator_tag;
		//! @private
		using difference_type = int;
		//! @private
		using value_type = T;
		//! @private
		using pointer = value_type*;
		//! @private
		using reference = value_type&;

		//! Default constructor
		inline iterator_base() = default;
		//! Copy constructor
		inline iterator_base(const iterator_base<T> &other) = default;
		//! Copy assignment operator
		inline iterator_base &operator=(const iterator_base<T> &other) = default;
		//! Move constructor
		inline iterator_base(iterator_base<T> &&other) noexcept = default;
		//! Move assignment operator
		inline iterator_base &operator=(iterator_base<T> &&other) noexcept = default;

		//! Posfix increment operator
		inline iterator_base<T> &operator++() { ++_index; return *this; }
		//! Suffix increment operator
		inline iterator_base<T> operator++(int) { return iterator_base<T>{_node, _index++}; }
		//! Posfix decrement operator
		inline iterator_base<T> &operator--() { --_index; return *this; }
		//! Suffix decrement operator
		inline iterator_base<T> operator--(int) { return iterator_base<T>{_node, _index--}; }

		//! Indirection operator
		inline reference operator*() const { return _node->at(_index); }
		//! Member access operator
		inline pointer operator->() const { return &_node->at(_index); }
		//! Swap operator
		inline friend void swap(iterator_base<T> &lhs, iterator_base<T> &rhs) noexcept { std::swap(lhs._node, rhs._node); std::swap(lhs._index, rhs._index); }

		//! Equality operator
		inline friend bool operator==(const iterator_base<T> &lhs, const iterator_base<T> &rhs) { assert(lhs._node == rhs._node); return lhs._index == rhs._index; }
		//! Inequality operator
		inline friend bool operator!=(const iterator_base<T> &lhs, const iterator_base<T> &rhs) { assert(lhs._node == rhs._node); return lhs._index != rhs._index; }
		//! Lesser than operator
		inline friend bool operator<(const iterator_base<T> &lhs, const iterator_base<T> &rhs) { assert(lhs._node == rhs._node); return lhs._index < rhs._index; }
		//! Geater than operator
		inline friend bool operator>(const iterator_base<T> &lhs, const iterator_base<T> &rhs) { assert(lhs._node == rhs._node); return lhs._index > rhs._index; }
		//! Lesser equal than operator
		inline friend bool operator<=(const iterator_base<T> &lhs, const iterator_base<T> &rhs) { assert(lhs._node == rhs._node); return lhs._index <= rhs._index; }
		//! Geater equal than operator
		inline friend bool operator>=(const iterator_base<T> &lhs, const iterator_base<T> &rhs) { assert(lhs._node == rhs._node); return lhs._index >= rhs._index; }

		//! Compound assignment operator
		inline iterator_base<T> &operator+=(difference_type delta) { _index += delta; return *this; }
		//! Addition operator
		inline friend iterator_base<T> operator+(const iterator_base<T> &iter, difference_type delta) { return iterator_base<T>{iter._node, iter._index + delta}; }
		//! Addition operator
		inline friend iterator_base<T> operator+(difference_type delta, const iterator_base<T> &iter) { return iterator_base<T>{iter._node, iter._index + delta}; }
		//! Compound assignment operator
		inline iterator_base<T> &operator-=(difference_type delta) { _index -= delta; return *this; }
		//! Substraction operator
		inline friend iterator_base<T> operator-(const iterator_base<T> &iter, difference_type delta) { return iterator_base<T>{iter._node, iter._index - delta}; }
		//! Substraction operator
		inline friend difference_type operator-(const iterator_base<T> &lhs, const iterator_base<T> &rhs) { assert(lhs._node == rhs._node); return lhs._index - rhs._index; }

		//! Subscript operator
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

	//! Typedef for a read and write iterator
	using iterator = iterator_base<TType>;
	//! Typedef for a readonly iterator
	using const_iterator = iterator_base<const TType>;

	SettingsListNode() = default;

	//! Checks if the entry that represents the list node is stored in the settings
	bool isSet() const;
	//! Returns the key this list node uses to access the settings
	QString key() const;
	//! Returns the current size of the list, as stored in the settings
	int size() const;
	//! @copybrief SettingsListNode::at(int)
	const TType &at(int index) const;
	//! Returns a reference to the element at the given position in the list
	TType &at(int index);

	//! Appends a new element to the list and returns a reference to that entry
	TType &push();
	//! Creates a new element and returns it, but does not push it yet
	Deferred push_deferred();
	//! Removes count elements from the back of the list from the settings
	void pop(int count = 1);
	//! Removes the list and all of its elements from the settings
	void reset();

	//! @copybrief SettingsListNode::at(int)
	TType &operator[](int index);
	//! @copybrief SettingsListNode::at(int)
	const TType &operator[](int index) const;

	//! returns an iterator pointing to the beginning of the list
	iterator begin();
	//! returns an iterator pointing after the end of the list
	iterator end();
	//! @copybrief SettingsListNode::begin()
	const_iterator begin() const;
	//! @copybrief SettingsListNode::end()
	const_iterator end() const;
	//! @copybrief SettingsListNode::begin()
	const_iterator constBegin() const;
	//! @copybrief SettingsListNode::end()
	const_iterator constEnd() const;

	//! Adds a method to be called if the number of elements stored for the list change
	void addChangeCallback(const std::function<void(int)> &callback); // size
	//! Adds a method to be called if the number of elements stored for the list change, as long as scope exists
	void addChangeCallback(QObject *scope, const std::function<void(int)> &callback);

	//! @private
	void setup(QString key, ISettingsAccessor *accessor, std::function<void(int, TType&)> setupFn);
	//! @private
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

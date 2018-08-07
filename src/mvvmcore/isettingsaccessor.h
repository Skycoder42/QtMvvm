#ifndef QTMVVM_ISETTINGSACCESSOR_H
#define QTMVVM_ISETTINGSACCESSOR_H

#include <type_traits>

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qvariant.h>

#include "QtMvvmCore/qtmvvmcore_global.h"

namespace QtMvvm {

//! An interface to provide a generic way to access settings of any origin
class Q_MVVMCORE_EXPORT ISettingsAccessor : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ISettingsAccessor)

public:
	// Set the default accessor type to be used by generated settings if none was specified
	template <typename T>
	static void setDefaultAccessor();
	//! @copybrief ISettingsAccessor::setDefaultAccessor
	static void setDefaultAccessor(int typeId);

	//! Create a new instance of the default accessor type
	static ISettingsAccessor *createDefaultAccessor(QObject *parent = nullptr);

	//! Constructor
	ISettingsAccessor(QObject *parent = nullptr);

	//! Checks if a value is stored for the given key
	virtual bool contains(const QString &key) const = 0;
	//! Loads the value for the given key from the settings
	virtual QVariant load(const QString &key, const QVariant &defaultValue = {}) const = 0;
	//! Stores the given value under the given key in the settings
	virtual void save(const QString &key, const QVariant &value) = 0;
	//! Removes the key and all its subkeys from the settings
	virtual void remove(const QString &key) = 0;

public Q_SLOTS:
	//! Synchronizes the settings to the disk or whatever is needed to permanently store them
	virtual void sync() = 0;

Q_SIGNALS:
	//! Is emitted whenever a settings value was changed, at least via this instance
	void entryChanged(const QString &key, const QVariant &value);
	//! Is emitted whenever a settings value was removed, at least via this instance
	void entryRemoved(const QString &key);

private:
	static int _DefaultAccessorType;
};

template<typename T>
void ISettingsAccessor::setDefaultAccessor()
{
	static_assert(std::is_base_of<T, ISettingsAccessor>::value, "T must implement the QtMvvm::ISettingsAccessor interface");
	setDefaultAccessor(qMetaTypeId<T*>());
}

}

//! The IID of the QtMvvm::IPresenter class
#define QtMvvm_ISettingsAccessorIID "de.skycoder42.qtmvvm.core.ISettingsAccessor"
Q_DECLARE_INTERFACE(QtMvvm::ISettingsAccessor, QtMvvm_ISettingsAccessorIID)
Q_DECLARE_METATYPE(QtMvvm::ISettingsAccessor*)

//! @file isettingsaccessor.h The ISettingsAccessor class header
#endif // QTMVVM_ISETTINGSACCESSOR_H

#ifndef QTMVVM_ANDROIDSETTINGSACCESSOR_H
#define QTMVVM_ANDROIDSETTINGSACCESSOR_H

#include <QtCore/qscopedpointer.h>

#include "QtMvvmCore/qtmvvmcore_global.h"
#include "QtMvvmCore/isettingsaccessor.h"

namespace QtMvvm {

class AndroidSettingsAccessorPrivate;
//! A wrapper around the android SharedPreferences API
class Q_MVVMCORE_EXPORT AndroidSettingsAccessor : public ISettingsAccessor
{
	Q_OBJECT
	Q_INTERFACES(QtMvvm::ISettingsAccessor)

public:
	//! Flags to specify how the shared preferences should be created
	enum ModeFlag {
		Private = 0x00000000, //!< The default. Only the application itself can access settings
		WorldReadable = 0x00000001, //!< Deprecated. Everyone canwrite the settings
		MultiProcess = 0x00000004 //!< Deprecated. Multiple process can use an instance in parallel
	};
	Q_DECLARE_FLAGS(Mode, ModeFlag)
	Q_FLAG(Mode)

	//! Default constructor
	Q_INVOKABLE explicit AndroidSettingsAccessor(QObject *parent = nullptr);
	//! Constructor with the name of the shared preferences to be opened
	explicit AndroidSettingsAccessor(const QString &file, QObject *parent = nullptr);
	//! Constructor with the name of the shared preferences to be opened and the mode to open them
	explicit AndroidSettingsAccessor(const QString &file, Mode mode, QObject *parent = nullptr);
	~AndroidSettingsAccessor() override;

	bool contains(const QString &key) const override;
	QVariant load(const QString &key, const QVariant &defaultValue = {}) const override;
	void save(const QString &key, const QVariant &value) override;
	void remove(const QString &key) override;

public Q_SLOTS:
	//! @copydoc ISettingsAccessor::sync
	void sync() override;

private Q_SLOTS:
	void changeCallback(const QString &key);

private:
	QScopedPointer<AndroidSettingsAccessorPrivate> d;
};

}

Q_DECLARE_METATYPE(QtMvvm::AndroidSettingsAccessor*)
Q_DECLARE_OPERATORS_FOR_FLAGS(QtMvvm::AndroidSettingsAccessor::Mode)

#endif // QTMVVM_ANDROIDSETTINGSACCESSOR_H

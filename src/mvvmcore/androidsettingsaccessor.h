#ifndef QTMVVM_ANDROIDSETTINGSACCESSOR_H
#define QTMVVM_ANDROIDSETTINGSACCESSOR_H

#include <QtCore/qscopedpointer.h>

#include "QtMvvmCore/qtmvvmcore_global.h"
#include "QtMvvmCore/isettingsaccessor.h"

namespace QtMvvm {

class AndroidSettingsAccessorPrivate;
class Q_MVVMCORE_EXPORT AndroidSettingsAccessor : public ISettingsAccessor
{
	Q_OBJECT
	Q_INTERFACES(QtMvvm::ISettingsAccessor)

public:
	enum ModeFlag {
		Private = 0x00000000,
		WorldReadable = 0x00000001,
		WorldWritable = 0x00000002,
		MultiProcess = 0x00000004
	};
	Q_DECLARE_FLAGS(Mode, ModeFlag)
	Q_FLAG(Mode)

	explicit AndroidSettingsAccessor(QObject *parent = nullptr);
	explicit AndroidSettingsAccessor(const QString &file, QObject *parent = nullptr);
	explicit AndroidSettingsAccessor(const QString &file, Mode mode, QObject *parent = nullptr);
	~AndroidSettingsAccessor() override;

	bool contains(const QString &key) const override;
	QVariant load(const QString &key, const QVariant &defaultValue = {}) const override;
	void save(const QString &key, const QVariant &value) override;
	void remove(const QString &key) override;

public Q_SLOTS:
	void sync() override;

private Q_SLOTS:
	void changeCallback(const QString &key);

private:
	QScopedPointer<AndroidSettingsAccessorPrivate> d;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(QtMvvm::AndroidSettingsAccessor::Mode)

#endif // QTMVVM_ANDROIDSETTINGSACCESSOR_H

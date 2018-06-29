#ifndef QTMVVM_ISETTINGSACCESSOR_H
#define QTMVVM_ISETTINGSACCESSOR_H

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qvariant.h>

#include "QtMvvmCore/qtmvvmcore_global.h"

namespace QtMvvm {

class Q_MVVMCORE_EXPORT ISettingsAccessor : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ISettingsAccessor)

public:
	ISettingsAccessor(QObject *parent = nullptr);

	virtual bool contains(const QString &key) const = 0;
	virtual QVariant load(const QString &key, const QVariant &defaultValue = {}) const = 0;
	virtual void save(const QString &key, const QVariant &value) = 0;
	virtual void remove(const QString &key) = 0;

public Q_SLOTS:
	virtual void sync() = 0;

Q_SIGNALS:
	void entryChanged(const QString &key, const QVariant &value);
	void entryRemoved(const QString &key);
};

}

#define ISettingsAccessorIid "de.skycoder42.qtmvvm.core.ISettingsAccessor"
Q_DECLARE_INTERFACE(QtMvvm::ISettingsAccessor, ISettingsAccessorIid)
Q_DECLARE_METATYPE(QtMvvm::ISettingsAccessor*)

#endif // QTMVVM_ISETTINGSACCESSOR_H

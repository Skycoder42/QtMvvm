#ifndef QTMVVM_QSETTINGSACCESSOR_H
#define QTMVVM_QSETTINGSACCESSOR_H

#include <QtCore/qsettings.h>
#include <QtCore/qscopedpointer.h>

#include "QtMvvmCore/qtmvvmcore_global.h"
#include "QtMvvmCore/isettingsaccessor.h"

namespace QtMvvm {

class QSettingsAccessorPrivate;
//! A settings accessor implementation that wrapps QSettings
class Q_MVVMCORE_EXPORT QSettingsAccessor : public ISettingsAccessor
{
	Q_OBJECT
	Q_INTERFACES(QtMvvm::ISettingsAccessor)

public:
	//! Default Constructor
	Q_INVOKABLE explicit QSettingsAccessor(QObject *parent = nullptr);
	//! Constructor, with settings to be used (takes ownership of the settings)
	explicit QSettingsAccessor(QSettings *settings, QObject *parent = nullptr);
	~QSettingsAccessor() override;

	bool contains(const QString &key) const override;
	QVariant load(const QString &key, const QVariant &defaultValue) const override;
	void save(const QString &key, const QVariant &value) override;
	void remove(const QString &key) override;

	//! Returns the internally used settings
	QSettings *settings() const;

public Q_SLOTS:
	//! @copydoc ISettingsAccessor::sync
	void sync() override;

private:
	QScopedPointer<QSettingsAccessorPrivate> d;
};

}

Q_DECLARE_METATYPE(QtMvvm::QSettingsAccessor*)

#endif // QTMVVM_QSETTINGSACCESSOR_H

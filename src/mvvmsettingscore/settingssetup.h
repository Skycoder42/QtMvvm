#ifndef QTMVVM_SETTINGSSETUP_H
#define QTMVVM_SETTINGSSETUP_H

#include <QtCore/qstringlist.h>
#include <QtCore/qvariant.h>
#include <QtCore/qurl.h>
#include <QtCore/qobject.h>
#include <QtCore/qfileselector.h>
#include <QtCore/qexception.h>

#include "QtMvvmSettingsCore/qtmvvmsettingscore_global.h"

namespace QtMvvm {

namespace SettingsElements {

struct SettingsEntry
{
	QString key;
	QByteArray type;
	QString title;
	QString tooltip;
	QVariant defaultValue;
	QStringList searchKeys;
	QVariantMap properties;

	QString frontends;
	QString selectors;
};

struct SettingsGroup
{
	QString title;
	QString tooltip;

	QList<SettingsEntry> entries;

	QString frontends;
	QString selectors;
};

struct SettingsSection
{
	QString title;
	QUrl icon;
	QString tooltip;

	QList<SettingsGroup> groups;

	QString frontends;
	QString selectors;
};

struct SettingsCategory
{
	QString title;
	QUrl icon;
	QString tooltip;

	QList<SettingsSection> sections;

	QString frontends;
	QString selectors;
};

struct SettingsSetup
{
	bool allowSearch = false;
	bool allowRestore = false;

	QList<SettingsCategory> categories;
};

}

class SettingsLoaderException : public QException {};

class ISettingsSetupLoader
{
public:
	virtual inline ~ISettingsSetupLoader() = default;

	virtual SettingsElements::SettingsSetup loadSetup(const QString &filePath, const QString &frontend, const QFileSelector *selector) const = 0;
};

}

#define QtMvvm_ISettingsSetupLoaderIid "de.skycoder42.qtmvvm.settings.core.ISettingsSetupLoader"
Q_DECLARE_INTERFACE(QtMvvm::ISettingsSetupLoader, QtMvvm_ISettingsSetupLoaderIid)
Q_DECLARE_METATYPE(QtMvvm::ISettingsSetupLoader*)

#endif // QTMVVM_SETTINGSSETUP_H

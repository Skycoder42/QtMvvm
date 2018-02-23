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

struct Entry
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

struct Group
{
	QString title;
	QString tooltip;

	QList<Entry> entries;

	QString frontends;
	QString selectors;
};

struct Section
{
	QString title;
	QUrl icon;
	QString tooltip;

	QList<Group> groups;

	QString frontends;
	QString selectors;
};

struct Category
{
	QString title;
	QUrl icon;
	QString tooltip;

	QList<Section> sections;

	QString frontends;
	QString selectors;
};

struct Setup
{
	bool allowSearch = true;
	bool allowRestore = true;

	QList<Category> categories;
};

}

class SettingsLoaderException : public QException {};

class ISettingsSetupLoader
{
public:
	virtual inline ~ISettingsSetupLoader() = default;

	virtual void changeDefaultIcon(const QUrl &defaultIcon) = 0;
	virtual SettingsElements::Setup loadSetup(const QString &filePath, const QString &frontend, const QFileSelector *selector) const = 0;
};

}

#define QtMvvm_ISettingsSetupLoaderIid "de.skycoder42.qtmvvm.settings.core.ISettingsSetupLoader"
Q_DECLARE_INTERFACE(QtMvvm::ISettingsSetupLoader, QtMvvm_ISettingsSetupLoaderIid)
Q_DECLARE_METATYPE(QtMvvm::ISettingsSetupLoader*)

#endif // QTMVVM_SETTINGSSETUP_H

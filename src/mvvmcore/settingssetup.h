#ifndef QTMVVM_SETTINGSSETUP_H
#define QTMVVM_SETTINGSSETUP_H

#include <QtCore/qstringlist.h>
#include <QtCore/qvariant.h>
#include <QtCore/qurl.h>
#include <QtCore/qobject.h>
#include <QtCore/qfileselector.h>

#include "QtMvvmCore/qtmvvmcore_global.h"
#include "QtMvvmCore/exception.h"

namespace QtMvvm {

//! A sub namespace for the different elements in a settings setup
namespace SettingsElements {

//! An entry in the settings to display and edit a value
struct Entry
{
	//! The QSettings key this entry should handle
	QString key;
	//! The input view type (typically the actual data type) to show
	QByteArray type;
	//! A short title/label of the entry to give it a name
	QString title;
	//! A tooltip with more detailed information about the entry
	QString tooltip;
	//! A default value to display in case no value is in the settings yet
	QVariant defaultValue;
	//! A list of keys to find this entry by when searching
	QStringList searchKeys;
	//! Additional properties to be passed on to the edit view
	QVariantMap properties;

	//! *[Internal]* A logical string describing the allowed frontends
	QString frontends; //MAJOR remove all
	//! *[Internal]* A logical string describing the allowed selectors
	QString selectors;
};

//! A logical group of settings entries
struct Group
{
	//! The groups title
	QString title;
	//! A tooltip to describe what the group is about
	QString tooltip;

	//! A list of entries the group contains
	QList<Entry> entries;

	//! *[Internal]* A logical string describing the allowed frontends
	QString frontends;
	//! *[Internal]* A logical string describing the allowed selectors
	QString selectors;
};

//! A bigger section for different entries and groups
struct Section
{
	//! The sections title
	QString title;
	//! An icon for the section
	QUrl icon;
	//! A tooltip to describe what the section is about
	QString tooltip;

	//! A list of groups the section contains
	QList<Group> groups;

	//! *[Internal]* A logical string describing the allowed frontends
	QString frontends;
	//! *[Internal]* A logical string describing the allowed selectors
	QString selectors;
};

//! A top level category to organize sections in
struct Category
{
	//! The categories title
	QString title;
	//! An icon for the category
	QUrl icon;
	//! A tooltip to describe what the category is about
	QString tooltip;

	//! A list of sections the category contains
	QList<Section> sections;

	//! *[Internal]* A logical string describing the allowed frontends
	QString frontends;
	//! *[Internal]* A logical string describing the allowed selectors
	QString selectors;
};

//! The whole settings setup
struct Setup
{
	//! Specifies if the user is allowed to search in the settings
	bool allowSearch = true;
	//! Specifies if the user is allowed to restore the factory defaults
	bool allowRestore = true;

	//! A list of categories the setup is build of
	QList<Category> categories;
};

}

//! An exception throw in case loading a settings setup went wrong
class SettingsLoaderException : public QTMVVM_EXCEPTION_BASE {};

//! An interface for a generic settings setup loader
class ISettingsSetupLoader
{
	Q_DISABLE_COPY(ISettingsSetupLoader)
public:
	inline ISettingsSetupLoader() = default;
	virtual inline ~ISettingsSetupLoader() = default;

	//! Can be used to overwrite the default icon for categories
	virtual void changeDefaultIcon(const QUrl &defaultIcon) = 0;
	//! Loads the settings setup from the given file
	virtual SettingsElements::Setup loadSetup(const QString &filePath, const QString &frontend, const QFileSelector *selector) const = 0; //MAJOR remove const
};

}

Q_DECLARE_METATYPE(QtMvvm::SettingsElements::Entry)
Q_DECLARE_METATYPE(QtMvvm::SettingsElements::Group)
Q_DECLARE_METATYPE(QtMvvm::SettingsElements::Section)
Q_DECLARE_METATYPE(QtMvvm::SettingsElements::Category)
Q_DECLARE_METATYPE(QtMvvm::SettingsElements::Setup)

//! The Iid of the QtMvvm::ISettingsSetupLoader class
#define QtMvvm_ISettingsSetupLoaderIid "de.skycoder42.qtmvvm.settings.core.ISettingsSetupLoader"
Q_DECLARE_INTERFACE(QtMvvm::ISettingsSetupLoader, QtMvvm_ISettingsSetupLoaderIid)
Q_DECLARE_METATYPE(QtMvvm::ISettingsSetupLoader*)

//! @file settingssetup.h The settings setup header
#endif // QTMVVM_SETTINGSSETUP_H

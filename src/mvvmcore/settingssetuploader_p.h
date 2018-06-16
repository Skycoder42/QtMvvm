#ifndef QTMVVM_SETTINGSSETUPLOADER_P_H
#define QTMVVM_SETTINGSSETUPLOADER_P_H

#include <tuple>

#include <QtCore/QObject>
#include <QtCore/QIODevice>
#include <QtCore/QCache>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QFile>

#include "qtmvvmcore_global.h"
#include "settingssetup.h"

namespace QtMvvm {

class Q_MVVMCORE_EXPORT SettingsSetupLoader : public QObject, public ISettingsSetupLoader
{
	Q_OBJECT
	Q_INTERFACES(QtMvvm::ISettingsSetupLoader)

public:
	Q_INVOKABLE SettingsSetupLoader(QObject *parent = nullptr);

	void changeDefaultIcon(const QUrl &defaultIcon) override;
	SettingsElements::Setup loadSetup(const QString &filePath, const QString &frontend, const QFileSelector *selector) const final;

	bool event(QEvent *event) override;

private:
	QUrl _defaultIcon{QStringLiteral("qrc:/de/skycoder42/qtmvvm/icons/settings.svg")};
	mutable QCache<QString, SettingsElements::Setup> _cache;

	//Functions to read the settings XML
	SettingsElements::Category readCategory(QXmlStreamReader &reader, const QFileSelector *selector) const;
	SettingsElements::Category readDefaultCategory(QXmlStreamReader &reader, const QFileSelector *selector) const;
	void readCategoryChildren(QXmlStreamReader &reader, SettingsElements::Category &category, const QFileSelector *selector) const;

	SettingsElements::Section readSection(QXmlStreamReader &reader, const QFileSelector *selector) const;
	SettingsElements::Section readDefaultSection(QXmlStreamReader &reader, const QFileSelector *selector) const;
	void readSectionChildren(QXmlStreamReader &reader, SettingsElements::Section &section, const QFileSelector *selector) const;

	SettingsElements::Group readGroup(QXmlStreamReader &reader, const QFileSelector *selector) const;
	SettingsElements::Group readDefaultGroup(QXmlStreamReader &reader, const QFileSelector *selector) const;
	void readGroupChildren(QXmlStreamReader &reader, SettingsElements::Group &group, const QFileSelector *selector) const;

	SettingsElements::Entry readEntry(QXmlStreamReader &reader) const;

	SettingsElements::Category createDefaultCategory() const;
	SettingsElements::Section createDefaultSection() const;

	std::tuple<QString, QVariant> readProperty(QXmlStreamReader &reader) const;
	QVariant readElement(QXmlStreamReader &reader) const;

	//Functions to read included files
	bool readCategoryInclude(QXmlStreamReader &reader, SettingsElements::Category &category, const QFileSelector *selector) const;
	bool readSectionInclude(QXmlStreamReader &reader, SettingsElements::Section &section, const QFileSelector *selector) const;
	bool readGroupInclude(QXmlStreamReader &reader, SettingsElements::Group &group, const QFileSelector *selector) const;
	bool readEntryInclude(QXmlStreamReader &reader, SettingsElements::Entry &entry, const QFileSelector *selector) const;
	bool readInclude(QXmlStreamReader &reader, const std::function<void(QXmlStreamReader&)> &readFn, const QString &typeName, const QFileSelector *selector) const;

	//Functions to filter the elements
	void clearSetup(SettingsElements::Setup &setup, const QString &frontend, const QStringList &selectors) const;
	void clearCategory(SettingsElements::Category &category, const QString &frontend, const QStringList &selectors) const;
	void clearSection(SettingsElements::Section &section, const QString &frontend, const QStringList &selectors) const;
	void clearGroup(SettingsElements::Group &group, const QString &frontend, const QStringList &selectors) const;
	template <typename T>
	bool isUsable(const T &configElement, const QString &frontend, const QStringList &selectors) const;
};

class Q_MVVMCORE_EXPORT SettingsXmlException : public SettingsLoaderException
{
public:
	SettingsXmlException(const QXmlStreamReader &reader);
	SettingsXmlException(QXmlStreamReader &reader, const QByteArray &customError, bool forceOverwrite = false);
	SettingsXmlException(const QFile &fileError);

	const char *what() const noexcept override;

	void raise() const override;
	QException *clone() const override;

protected:
	SettingsXmlException(const SettingsXmlException * const other);

private:
	const QByteArray _what;
};

}

#endif // QTMVVM_SETTINGSSETUPLOADER_P_H

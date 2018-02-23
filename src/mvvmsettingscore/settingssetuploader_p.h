#ifndef QTMVVM_SETTINGSSETUPLOADER_P_H
#define QTMVVM_SETTINGSSETUPLOADER_P_H

#include <tuple>

#include <QtCore/QObject>
#include <QtCore/QIODevice>
#include <QtCore/QCache>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QFile>

#include "qtmvvmsettingscore_global.h"
#include "settingssetup.h"

namespace QtMvvm {

class SettingsSetupLoader : public QObject, public ISettingsSetupLoader
{
	Q_OBJECT
	Q_INTERFACES(QtMvvm::ISettingsSetupLoader)

public:
	Q_INVOKABLE SettingsSetupLoader(QObject *parent = nullptr);

	SettingsElements::SettingsSetup loadSetup(const QString &filePath, const QString &frontend, const QFileSelector *selector) const final;

	bool event(QEvent *event) override;

private:
	static QUrl defaultIcon;

	mutable QCache<QString, SettingsElements::SettingsSetup> _cache;

	//Functions to read the settings XML
	SettingsElements::SettingsCategory readCategory(QXmlStreamReader &reader) const;
	SettingsElements::SettingsCategory readDefaultCategory(QXmlStreamReader &reader) const;
	void readCategoryChildren(QXmlStreamReader &reader, SettingsElements::SettingsCategory &category) const;

	SettingsElements::SettingsSection readSection(QXmlStreamReader &reader) const;
	SettingsElements::SettingsSection readDefaultSection(QXmlStreamReader &reader) const;
	void readSectionChildren(QXmlStreamReader &reader, SettingsElements::SettingsSection &section) const;

	SettingsElements::SettingsGroup readGroup(QXmlStreamReader &reader) const;
	SettingsElements::SettingsGroup readDefaultGroup(QXmlStreamReader &reader) const;
	void readGroupChildren(QXmlStreamReader &reader, SettingsElements::SettingsGroup &group) const;

	SettingsElements::SettingsEntry readEntry(QXmlStreamReader &reader) const;

	SettingsElements::SettingsCategory createDefaultCategory() const;
	SettingsElements::SettingsSection createDefaultSection() const;

	std::tuple<QString, QVariant> readProperty(QXmlStreamReader &reader) const;
	QVariant readElement(QXmlStreamReader &reader) const;

	//Functions to read included files
	bool readCategoryInclude(QXmlStreamReader &reader, SettingsElements::SettingsCategory &category) const;
	bool readSectionInclude(QXmlStreamReader &reader, SettingsElements::SettingsSection &section) const;
	bool readGroupInclude(QXmlStreamReader &reader, SettingsElements::SettingsGroup &group) const;
	bool readEntryInclude(QXmlStreamReader &reader, SettingsElements::SettingsEntry &entry) const;
	bool readInclude(QXmlStreamReader &reader, const std::function<void(QXmlStreamReader&)> &readFn, const QString &typeName) const;

	//Functions to filter the elements
	void clearSetup(SettingsElements::SettingsSetup &setup, const QString &frontend, const QStringList &selectors) const;
	void clearCategory(SettingsElements::SettingsCategory &category, const QString &frontend, const QStringList &selectors) const;
	void clearSection(SettingsElements::SettingsSection &section, const QString &frontend, const QStringList &selectors) const;
	void clearGroup(SettingsElements::SettingsGroup &group, const QString &frontend, const QStringList &selectors) const;
	template <typename T>
	bool isUsable(const T &configElement, const QString &frontend, const QStringList &selectors) const;
};

class SettingsXmlException : public SettingsLoaderException
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

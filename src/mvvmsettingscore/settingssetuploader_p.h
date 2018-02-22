#ifndef QTMVVM_SETTINGSSETUPLOADER_P_H
#define QTMVVM_SETTINGSSETUPLOADER_P_H

#include <tuple>

#include <QtCore/QObject>
#include <QtCore/QIODevice>
#include <QtCore/QCache>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QException>
#include <QtCore/QFile>

#include "qtmvvmsettingscore_global.h"
#include "settingssetup.h"

namespace QtMvvm {

class SettingsSetupLoader : public QObject, public ISettingsSetupLoader
{
	Q_OBJECT
	Q_INTERFACES(QtMvvm::ISettingsSetupLoader)

public:
	SettingsSetupLoader(QObject *parent = nullptr);

	SettingsSetup loadSetup(const QString &platform, const QFileSelector *selector, const QString &filePath) const override;

private:
	static QUrl defaultIcon;

	mutable QCache<QString, SettingsSetup> _cache;

	//functions to read the settings XML
	SettingsCategory readCategory(QXmlStreamReader &reader) const;
	SettingsCategory readDefaultCategory(QXmlStreamReader &reader) const;

	SettingsSection readSection(QXmlStreamReader &reader) const;
	SettingsSection readDefaultSection(QXmlStreamReader &reader) const;

	SettingsGroup readGroup(QXmlStreamReader &reader) const;
	SettingsGroup readDefaultGroup(QXmlStreamReader &reader) const;

	SettingsEntry readEntry(QXmlStreamReader &reader) const;

	SettingsCategory createDefaultCategory() const;
	SettingsSection createDefaultSection() const;

	std::tuple<QString, QVariant> readProperty(QXmlStreamReader &reader) const;
	QVariant readElement(QXmlStreamReader &reader) const;

	//Functions to filter the elements
	void clearSetup(SettingsSetup &setup, const QString &frontend, const QStringList &selectors) const;
	void clearCategory(SettingsCategory &category, const QString &frontend, const QStringList &selectors) const;
	void clearSection(SettingsSection &section, const QString &frontend, const QStringList &selectors) const;
	void clearGroup(SettingsGroup &group, const QString &frontend, const QStringList &selectors) const;
	template <typename T>
	bool isUsable(const T &configElement, const QString &frontend, const QStringList &selectors) const;
};

class SettingsXmlException : public QException
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

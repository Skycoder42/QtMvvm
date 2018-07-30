#ifndef SETTINGSTRANSLATOR_H
#define SETTINGSTRANSLATOR_H

#include <QFile>
#include <QTextStream>

#include "settingsconfigimpl_p.h"

class SettingsTranslator : public SettingsConfigImpl
{
public:
	SettingsTranslator(const QString &srcPath);

	void process(const QString &inPath);

private:
	QFile _srcFile;
	QTextStream _src;

	void writeTranslations(const SettingsConfigType &settings, const QString &inPath);

	void readCategoryStrings(const CategoryType &category, QStringList &strings);
	void readSectionStrings(const SectionType &section, QStringList &strings);
	void readGroupStrings(const GroupType &group, QStringList &strings);
	void readEntryStrings(const EntryType &entry, QStringList &strings);

	void readInfoStrings(const BasicContainerInfo &info, QStringList &strings);
	void readElementStrings(const ElementType &element, QStringList &strings);
};

#endif // SETTINGSTRANSLATOR_H

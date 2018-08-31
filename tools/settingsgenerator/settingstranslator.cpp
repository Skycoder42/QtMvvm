#include "settingstranslator.h"
#include <QFileInfo>

SettingsTranslator::SettingsTranslator(const QString &srcPath) :
	_srcFile{srcPath},
	_src{&_srcFile}
{
	setIncludesOptional(true);
}

void SettingsTranslator::process(const QString &inPath)
{
	auto settings = readDocument(inPath);
	if(!nonstd::holds_alternative<SettingsConfigType>(settings))
		throw XmlException{inPath, 0, 0, QStringLiteral("Expected the root element to be a SettingsConfig element")};
	writeTranslations(nonstd::get<SettingsConfigType>(settings), inPath);
}

void SettingsTranslator::writeTranslations(const SettingsConfigType &settings, const QString &inPath)
{
	QStringList strings;
	for(auto &element : settings.content) {
		if(nonstd::holds_alternative<CategoryType>(element))
			readCategoryStrings(nonstd::get<CategoryType>(element), strings);
		else if(nonstd::holds_alternative<SectionType>(element))
			readSectionStrings(nonstd::get<SectionType>(element), strings);
		else if(nonstd::holds_alternative<GroupType>(element))
			readGroupStrings(nonstd::get<GroupType>(element), strings);
		else if(nonstd::holds_alternative<EntryType>(element))
			readEntryStrings(nonstd::get<EntryType>(element), strings);
		else
			Q_UNREACHABLE();
	}

	strings.removeDuplicates();

	if(!_srcFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throw FileException{_srcFile};
	auto context = QFileInfo{inPath}.fileName();

	_src << "#include <QtCore/QCoreApplication>\n\n"
		 << "namespace {\n\n"
		 << "void Q_DECL_UNUSED dummyfn()\n"
		 << "{\n";
	for(auto &string : strings) {
		_src << "\tQCoreApplication::translate(\"" << context << "\", \""
			 << string.replace(QLatin1Char('\\'), QStringLiteral("\\\\"))
					  .replace(QLatin1Char('\"'), QStringLiteral("\\\""))
			 << "\");\n";
	}
	_src << "}\n\n"
		 << "}\n";

	_src.flush();
	_srcFile.close();
}

void SettingsTranslator::readCategoryStrings(const CategoryType &category, QStringList &strings)
{
	readInfoStrings(category, strings);
	for(auto &element : category.content) {
		if(nonstd::holds_alternative<SectionType>(element))
			readSectionStrings(nonstd::get<SectionType>(element), strings);
		else if(nonstd::holds_alternative<GroupType>(element))
			readGroupStrings(nonstd::get<GroupType>(element), strings);
		else if(nonstd::holds_alternative<EntryType>(element))
			readEntryStrings(nonstd::get<EntryType>(element), strings);
		else
			Q_UNREACHABLE();
	}
}

void SettingsTranslator::readSectionStrings(const SectionType &section, QStringList &strings)
{
	readInfoStrings(section, strings);
	for(auto &element : section.content) {
		if(nonstd::holds_alternative<GroupType>(element))
			readGroupStrings(nonstd::get<GroupType>(element), strings);
		else if(nonstd::holds_alternative<EntryType>(element))
			readEntryStrings(nonstd::get<EntryType>(element), strings);
		else
			Q_UNREACHABLE();
	}
}

void SettingsTranslator::readGroupStrings(const GroupType &group, QStringList &strings)
{
	readInfoStrings(group, strings);
	for(auto &element : group.content) {
		if(nonstd::holds_alternative<EntryType>(element))
			readEntryStrings(nonstd::get<EntryType>(element), strings);
		else
			Q_UNREACHABLE();
	}
}

void SettingsTranslator::readEntryStrings(const EntryType &entry, QStringList &strings)
{
	readInfoStrings(entry, strings);
	if(entry.trdefault && entry.defaultValue)
		strings.append(entry.defaultValue.value());
	strings.append(entry.searchKeys);
	for(const auto &prop : entry.properties)
		readElementStrings(prop, strings);
}

void SettingsTranslator::readInfoStrings(const BasicContainerInfo &info, QStringList &strings)
{
	if(info.title)
		strings.append(info.title.value());
	if(info.tooltip)
		strings.append(info.tooltip.value());
}

void SettingsTranslator::readElementStrings(const ElementType &element, QStringList &strings)
{
	if(element.content && (element.tr || element.ztr))
		strings.append(element.content.value());
	for(const auto &prop : element.elements)
		readElementStrings(prop, strings);
	for(const auto &prop : element.properties)
		readElementStrings(prop, strings);
}

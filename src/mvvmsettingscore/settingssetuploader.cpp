#include "settingssetuploader_p.h"
#include <QtCore/QCoreApplication>
using namespace QtMvvm;
using namespace QtMvvm::SettingsElements;

#define throwXmlError(...) throw SettingsXmlException(__VA_ARGS__)
#define throwAttrib(reader, attrib) throwXmlError(reader, "Attribute \"" attrib "\" is required, but was not set!")
#define throwElement(reader, element) throwXmlError(reader, "Expected element of type <" element ">")

#define testXmlValid(reader) if(reader.hasError()) throwXmlError(reader)

#define hasValue(key) attributes().hasAttribute(QStringLiteral(key))
#define stringValue(key) attributes().value(QStringLiteral(key)).toString()
#define boolValue(key) attributes().value(QStringLiteral(key)) == QStringLiteral("true")

#define trctx(x) QCoreApplication::translate("qtmvvm_settings_xml", qUtf8Printable(x))

QUrl SettingsSetupLoader::defaultIcon(QStringLiteral("qrc:/qtmvvm/icons/settings.svg"));

SettingsSetupLoader::SettingsSetupLoader(QObject *parent) :
	QObject(parent),
	_cache()
{}

SettingsSetup SettingsSetupLoader::loadSetup(const QString &filePath, const QString &frontend, const QFileSelector *selector) const
{
	SettingsSetup setup;
	if(!_cache.contains(filePath)) {
		QFile file(filePath);
		if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
			throw SettingsXmlException(file);

		QXmlStreamReader reader(&file);
		testXmlValid(reader);

		if(!reader.readNextStartElement() || reader.name() != QStringLiteral("SettingsConfig"))
			throwElement(reader, "SettingsConfig");
		setup.allowSearch = reader.boolValue("allowSearch");
		setup.allowRestore = reader.boolValue("allowRestore");

		if(reader.readNextStartElement()) {
			if(reader.name() == QStringLiteral("Category")) {
				do
					setup.categories.append(readCategory(reader));
				while(reader.readNextStartElement());
			} else
				setup.categories.append(readDefaultCategory(reader));
		}
		testXmlValid(reader);
		file.close();

		_cache.insert(filePath, new SettingsSetup(setup));
	} else
		setup = *(_cache.object(filePath));

	clearSetup(setup, frontend, selector->allSelectors());
	return setup;
}

bool SettingsSetupLoader::event(QEvent *event)
{
	//reset the loaded cache on language changes
	switch(event->type()) {
	case QEvent::LanguageChange:
	case QEvent::LocaleChange:
		_cache.clear();
		break;
	default:
		break;
	}

	return QObject::event(event);
}

SettingsCategory SettingsSetupLoader::readCategory(QXmlStreamReader &reader) const
{
	testXmlValid(reader);
	if(reader.name() != QStringLiteral("Category"))
		throwElement(reader, "Category");

	auto category = createDefaultCategory();

	if(reader.hasValue("title"))
		category.title = trctx(reader.stringValue("title"));
	if(reader.hasValue("icon"))
		category.icon = reader.stringValue("icon");
	if(reader.hasValue("tooltip"))
		category.tooltip = trctx(reader.stringValue("tooltip"));
	category.frontends = reader.stringValue("frontends");
	category.selectors = reader.stringValue("selectors");

	if(reader.readNextStartElement()) {
		if(reader.name() == QStringLiteral("Section")) {
			do
				category.sections.append(readSection(reader));
			while(reader.readNextStartElement());
		} else
			category.sections.append(readDefaultSection(reader));
	}
	testXmlValid(reader);

	return category;
}

SettingsCategory SettingsSetupLoader::readDefaultCategory(QXmlStreamReader &reader) const
{
	testXmlValid(reader);
	auto category = createDefaultCategory();
	if(reader.name() == QStringLiteral("Section")) {
		do
			category.sections.append(readSection(reader));
		while(reader.readNextStartElement());
	} else
		category.sections.append(readDefaultSection(reader));
	testXmlValid(reader);

	return category;
}

SettingsSection SettingsSetupLoader::readSection(QXmlStreamReader &reader) const
{
	testXmlValid(reader);
	if(reader.name() != QStringLiteral("Section"))
		throwElement(reader, "Section");

	auto section = createDefaultSection();

	if(reader.hasValue("title"))
		section.title = trctx(reader.stringValue("title"));
	if(reader.hasValue("icon"))
		section.icon = reader.stringValue("icon");
	if(reader.hasValue("tooltip"))
		section.tooltip = trctx(reader.stringValue("tooltip"));
	section.frontends = reader.stringValue("frontends");
	section.selectors = reader.stringValue("selectors");

	if(reader.readNextStartElement()) {
		if(reader.name() == QStringLiteral("Group")) {
			do
				section.groups.append(readGroup(reader));
			while(reader.readNextStartElement());
		} else
			section.groups.append(readDefaultGroup(reader));
	}
	testXmlValid(reader);

	return section;
}

SettingsSection SettingsSetupLoader::readDefaultSection(QXmlStreamReader &reader) const
{
	testXmlValid(reader);
	auto section = createDefaultSection();

	if(reader.name() == QStringLiteral("Group")) {
		do
			section.groups.append(readGroup(reader));
		while(reader.readNextStartElement());
	} else
		section.groups.append(readDefaultGroup(reader));
	testXmlValid(reader);

	return section;
}

SettingsGroup SettingsSetupLoader::readGroup(QXmlStreamReader &reader) const
{
	testXmlValid(reader);
	if(reader.name() != QStringLiteral("Group"))
		throwElement(reader, "Group");

	SettingsGroup group;
	if(reader.hasValue("title"))
		group.title = trctx(reader.stringValue("title"));
	if(reader.hasValue("tooltip"))
		group.tooltip = trctx(reader.stringValue("tooltip"));
	group.frontends = reader.stringValue("frontends");
	group.selectors = reader.stringValue("selectors");

	if(reader.readNextStartElement()) {
		if(reader.name() == QStringLiteral("Entry")) {
			do
				group.entries.append(readEntry(reader));
			while(reader.readNextStartElement());
		} else
			throwElement(reader, "Entry");
	}
	testXmlValid(reader);

	return group;
}

SettingsGroup SettingsSetupLoader::readDefaultGroup(QXmlStreamReader &reader) const
{
	testXmlValid(reader);
	SettingsGroup group;

	if(reader.name() == QStringLiteral("Entry")) {
		do
			group.entries.append(readEntry(reader));
		while(reader.readNextStartElement());
	} else
		throwElement(reader, "Entry");
	testXmlValid(reader);

	return group;
}

SettingsEntry SettingsSetupLoader::readEntry(QXmlStreamReader &reader) const
{
	testXmlValid(reader);
	SettingsEntry entry;

	if(!reader.hasValue("key"))
		throwAttrib(reader, "key");
	entry.key = reader.stringValue("key");
	if(!reader.hasValue("type"))
		throwAttrib(reader, "type");
	entry.type = reader.stringValue("type").toLatin1();
	if(!reader.hasValue("title"))
		throwAttrib(reader, "title");
	entry.title = trctx(reader.stringValue("title"));
	entry.tooltip = trctx(reader.stringValue("tooltip"));
	entry.defaultValue = trctx(reader.stringValue("default"));
	entry.frontends = reader.stringValue("frontends");
	entry.selectors = reader.stringValue("selectors");

	while(reader.readNextStartElement()) {
		if(reader.name() == QStringLiteral("SearchKey"))
			entry.searchKeys.append(trctx(reader.readElementText()));
		else if(reader.name() == QStringLiteral("Property")) {
			auto prop = readProperty(reader);
			entry.properties.insert(std::get<0>(prop), std::get<1>(prop));
		} else
			throwXmlError(reader, "Expected element of type <SearchKey> or <Property>");
	}

	return entry;
}

SettingsCategory SettingsSetupLoader::createDefaultCategory() const
{
	SettingsCategory category;
	category.title = tr("General Settings");
	category.icon = defaultIcon;
	return category;
}

SettingsSection SettingsSetupLoader::createDefaultSection() const
{
	SettingsSection section;
	section.title = tr("General");
	return section;
}

std::tuple<QString, QVariant> SettingsSetupLoader::readProperty(QXmlStreamReader &reader) const
{
	if(!reader.hasValue("key"))
		throwAttrib(reader, "key");
	return std::make_tuple(reader.stringValue("key"), readElement(reader));
}

QVariant SettingsSetupLoader::readElement(QXmlStreamReader &reader) const
{
	if(!reader.hasValue("type"))
		throwAttrib(reader, "type");
	auto type = reader.stringValue("type");

	//special type mappings
	if(type == QStringLiteral("string"))
		type = QString::fromUtf8(QMetaType::typeName(QMetaType::QString));
	else if(type == QStringLiteral("number"))
		type = QString::fromUtf8(QMetaType::typeName(QMetaType::Int));

	//special types
	if(type == QStringLiteral("list")) {
		QVariantList list;
		while (reader.readNextStartElement()) {
			if(reader.name() != QStringLiteral("Element"))
				throwElement(reader, "Element");
			list.append(readElement(reader));
		}
		testXmlValid(reader);
		return list;
	} else if(type == QStringLiteral("object")) {
		QVariantMap map;
		while (reader.readNextStartElement()) {
			if(reader.name() != QStringLiteral("Property"))
				throwElement(reader, "Property");
			auto property = readProperty(reader);
			map.insert(std::get<0>(property), std::get<1>(property));
		}
		testXmlValid(reader);
		return map;
	}

	//common types
	auto typeId = QMetaType::type(qUtf8Printable(type));
	if(typeId == QMetaType::UnknownType)
		throwXmlError(reader, "Unknown type: " + type.toUtf8());
	QVariant mVariant;
	if(reader.boolValue("tr"))
		mVariant = trctx(reader.readElementText());
	else
		mVariant = reader.readElementText();
	if(!mVariant.convert(typeId))
		throwXmlError(reader, "Failed to convert element data to type: " + type.toUtf8());

	return mVariant;
}

void SettingsSetupLoader::clearSetup(SettingsSetup &setup, const QString &frontend, const QStringList &selectors) const
{
	for(auto it = setup.categories.begin(); it != setup.categories.end();) {
		if(isUsable(*it, frontend, selectors)) {
			clearCategory(*it, frontend, selectors);
			it++;
		} else
			it = setup.categories.erase(it);
	}
}

void SettingsSetupLoader::clearCategory(SettingsCategory &category, const QString &frontend, const QStringList &selectors) const
{
	for(auto it = category.sections.begin(); it != category.sections.end();) {
		if(isUsable(*it, frontend, selectors)) {
			clearSection(*it, frontend, selectors);
			it++;
		} else
			it = category.sections.erase(it);
	}
}

void SettingsSetupLoader::clearSection(SettingsSection &section, const QString &frontend, const QStringList &selectors) const
{
	for(auto it = section.groups.begin(); it != section.groups.end();) {
		if(isUsable(*it, frontend, selectors)) {
			clearGroup(*it, frontend, selectors);
			it++;
		} else
			it = section.groups.erase(it);
	}
}

void SettingsSetupLoader::clearGroup(SettingsGroup &group, const QString &frontend, const QStringList &selectors) const
{
	for(auto it = group.entries.begin(); it != group.entries.end();) {
		if(isUsable(*it, frontend, selectors))
			it++;
		else
			it = group.entries.erase(it);
	}
}

template<typename T>
bool SettingsSetupLoader::isUsable(const T &configElement, const QString &frontend, const QStringList &selectors) const
{
	auto fronts = configElement.frontends.split(QLatin1Char('|'), QString::SkipEmptyParts);
	if(!fronts.isEmpty() && !fronts.contains(frontend))
		return false;

	auto selects = configElement.selectors.split(QLatin1Char('|'), QString::SkipEmptyParts);
	for(auto select : selects) {
		auto sels = select.split(QLatin1Char('&'), QString::SkipEmptyParts);
		auto allSelected = true;
		for(auto sel : sels) {
			if(!selectors.contains(sel)) {
				allSelected = false;
				break;
			}
		}

		if(allSelected)
			return true;
	}

	return selects.isEmpty();
}




SettingsXmlException::SettingsXmlException(const QXmlStreamReader &reader) :
	SettingsLoaderException(),
	_what(QStringLiteral("XML Error at %1:%2. Error: %3")
		  .arg(reader.lineNumber())
		  .arg(reader.columnNumber())
		  .arg(reader.errorString())
		  .toUtf8())
{}

SettingsXmlException::SettingsXmlException(QXmlStreamReader &reader, const QByteArray &customError, bool forceOverwrite) :
	SettingsXmlException([&]() -> QXmlStreamReader & {
		if(forceOverwrite || !reader.hasError())
			reader.raiseError(QString::fromUtf8(customError));
		return reader;
	}())
{}

SettingsXmlException::SettingsXmlException(const QFile &fileError) :
	SettingsLoaderException(),
	_what(QStringLiteral("Failed to open file \"%1\" with error: %2")
		  .arg(fileError.fileName())
		  .arg(fileError.errorString())
		  .toUtf8())
{}

SettingsXmlException::SettingsXmlException(const SettingsXmlException * const other) :
	SettingsLoaderException(),
	_what(other->_what)
{}

const char *SettingsXmlException::what() const noexcept
{
	return _what.constData();
}

void SettingsXmlException::raise() const
{
	throw (*this);
}

QException *SettingsXmlException::clone() const
{
	return new SettingsXmlException(this);
}

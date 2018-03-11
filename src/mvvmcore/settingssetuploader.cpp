#include "settingssetuploader_p.h"
#include "qtmvvm_logging_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>

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

SettingsSetupLoader::SettingsSetupLoader(QObject *parent) :
	QObject(parent),
	_defaultIcon(QStringLiteral("qrc:/de/skycoder42/qtmvvm/icons/settings.svg")),
	_cache()
{}

void SettingsSetupLoader::changeDefaultIcon(const QUrl &defaultIcon)
{
	_defaultIcon = defaultIcon;
}

Setup SettingsSetupLoader::loadSetup(const QString &filePath, const QString &frontend, const QFileSelector *selector) const
{
	Setup setup;
	if(!_cache.contains(filePath)) {
		QFile file(QDir::cleanPath(selector->select(filePath)));
		if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
			throw SettingsXmlException(file);

		QXmlStreamReader reader(&file);
		testXmlValid(reader);

		if(!reader.readNextStartElement() || reader.name() != QStringLiteral("SettingsConfig"))
			throwElement(reader, "SettingsConfig");

		if(reader.hasValue("allowSearch"))
			setup.allowSearch = reader.boolValue("allowSearch");
		if(reader.hasValue("allowRestore"))
			setup.allowRestore = reader.boolValue("allowRestore");

		if(reader.readNextStartElement()) {
			if(reader.name() == QStringLiteral("Category") ||
			   reader.name() == QStringLiteral("Include")) {
				do {
					if(reader.name() == QStringLiteral("Include")) {
						Category category;
						if(readCategoryInclude(reader, category, selector))
							setup.categories.append(category);
					} else
						setup.categories.append(readCategory(reader, selector));
				} while(reader.readNextStartElement());
			} else
				setup.categories.append(readDefaultCategory(reader, selector));
		}
		testXmlValid(reader);
		file.close();

		_cache.insert(filePath, new Setup(setup));
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

Category SettingsSetupLoader::readCategory(QXmlStreamReader &reader, const QFileSelector *selector) const
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

	if(reader.readNextStartElement())
		readCategoryChildren(reader, category, selector);
	testXmlValid(reader);

	return category;
}

Category SettingsSetupLoader::readDefaultCategory(QXmlStreamReader &reader, const QFileSelector *selector) const
{
	testXmlValid(reader);
	auto category = createDefaultCategory();
	readCategoryChildren(reader, category, selector);
	testXmlValid(reader);
	return category;
}

void SettingsSetupLoader::readCategoryChildren(QXmlStreamReader &reader, Category &category, const QFileSelector *selector) const
{
	if(reader.name() == QStringLiteral("Section") ||
	   reader.name() == QStringLiteral("Include")) {
		do {
			if(reader.name() == QStringLiteral("Include")) {
				Section section;
				if(readSectionInclude(reader, section, selector))
					category.sections.append(section);
			} else
				category.sections.append(readSection(reader, selector));
		} while(reader.readNextStartElement());
	} else
		category.sections.append(readDefaultSection(reader, selector));
}

Section SettingsSetupLoader::readSection(QXmlStreamReader &reader, const QFileSelector *selector) const
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

	if(reader.readNextStartElement())
		readSectionChildren(reader, section, selector);
	testXmlValid(reader);

	return section;
}

Section SettingsSetupLoader::readDefaultSection(QXmlStreamReader &reader, const QFileSelector *selector) const
{
	testXmlValid(reader);
	auto section = createDefaultSection();
	readSectionChildren(reader, section, selector);
	testXmlValid(reader);
	return section;
}

void SettingsSetupLoader::readSectionChildren(QXmlStreamReader &reader, Section &section, const QFileSelector *selector) const
{
	if(reader.name() == QStringLiteral("Group") ||
	   reader.name() == QStringLiteral("Include")) {
		do {
			if(reader.name() == QStringLiteral("Include")) {
				Group group;
				if(readGroupInclude(reader, group, selector))
					section.groups.append(group);
			} else
				section.groups.append(readGroup(reader, selector));
		} while(reader.readNextStartElement());
	} else
		section.groups.append(readDefaultGroup(reader, selector));
}

Group SettingsSetupLoader::readGroup(QXmlStreamReader &reader, const QFileSelector *selector) const
{
	testXmlValid(reader);
	if(reader.name() != QStringLiteral("Group"))
		throwElement(reader, "Group");

	Group group;
	if(reader.hasValue("title"))
		group.title = trctx(reader.stringValue("title"));
	if(reader.hasValue("tooltip"))
		group.tooltip = trctx(reader.stringValue("tooltip"));
	group.frontends = reader.stringValue("frontends");
	group.selectors = reader.stringValue("selectors");

	if(reader.readNextStartElement())
		readGroupChildren(reader, group, selector);
	testXmlValid(reader);

	return group;
}

Group SettingsSetupLoader::readDefaultGroup(QXmlStreamReader &reader, const QFileSelector *selector) const
{
	testXmlValid(reader);
	Group group;
	readGroupChildren(reader, group, selector);
	testXmlValid(reader);
	return group;
}

void SettingsSetupLoader::readGroupChildren(QXmlStreamReader &reader, Group &group, const QFileSelector *selector) const
{
	if(reader.name() == QStringLiteral("Entry") ||
	   reader.name() == QStringLiteral("Include")) {
		do {
			if(reader.name() == QStringLiteral("Include")) {
				Entry entry;
				if(readEntryInclude(reader, entry, selector))
					group.entries.append(entry);
			} else
				group.entries.append(readEntry(reader));
		} while(reader.readNextStartElement());
	} else
		throwXmlError(reader, QStringLiteral("Unexpected element type <%1>").arg(reader.name()).toUtf8());
}

Entry SettingsSetupLoader::readEntry(QXmlStreamReader &reader) const
{
	testXmlValid(reader);
	if(reader.name() != QStringLiteral("Entry"))
		throwElement(reader, "Entry");

	Entry entry;

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
		if(reader.name() == QStringLiteral("SearchKey")) {
			entry.searchKeys.append(trctx(reader.readElementText()));
			testXmlValid(reader);
		} else if(reader.name() == QStringLiteral("Property")) {
			auto prop = readProperty(reader);
			entry.properties.insert(std::get<0>(prop), std::get<1>(prop));
		} else
			throwXmlError(reader, "Expected element of type <SearchKey> or <Property>");
	}

	return entry;
}

Category SettingsSetupLoader::createDefaultCategory() const
{
	Category category;
	category.title = tr("General Settings");
	category.icon = _defaultIcon;
	return category;
}

Section SettingsSetupLoader::createDefaultSection() const
{
	Section section;
	section.title = tr("General");
	return section;
}

std::tuple<QString, QVariant> SettingsSetupLoader::readProperty(QXmlStreamReader &reader) const
{
	if(!reader.hasValue("key"))
		throwAttrib(reader, "key");
	auto key = reader.stringValue("key");
	return std::make_tuple(key, readElement(reader));
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
	testXmlValid(reader);
	if(!mVariant.convert(typeId))
		throwXmlError(reader, "Failed to convert element data to type: " + type.toUtf8());

	return mVariant;
}

bool SettingsSetupLoader::readCategoryInclude(QXmlStreamReader &reader, Category &category, const QFileSelector *selector) const
{
	return readInclude(reader, [this, &category, selector](QXmlStreamReader &reader){
		category = readCategory(reader, selector);
	}, QStringLiteral("Category"), selector);
}

bool SettingsSetupLoader::readSectionInclude(QXmlStreamReader &reader, Section &section, const QFileSelector *selector) const
{
	return readInclude(reader, [this, &section, selector](QXmlStreamReader &reader){
		section = readSection(reader, selector);
	}, QStringLiteral("Section"), selector);
}

bool SettingsSetupLoader::readGroupInclude(QXmlStreamReader &reader, Group &group, const QFileSelector *selector) const
{
	return readInclude(reader, [this, &group, selector](QXmlStreamReader &reader){
		group = readGroup(reader, selector);
	}, QStringLiteral("Group"), selector);
}

bool SettingsSetupLoader::readEntryInclude(QXmlStreamReader &reader, Entry &entry, const QFileSelector *selector) const
{
	return readInclude(reader, [this, &entry](QXmlStreamReader &reader){
		entry = readEntry(reader);
	}, QStringLiteral("Entry"), selector);
}

bool SettingsSetupLoader::readInclude(QXmlStreamReader &reader, const std::function<void(QXmlStreamReader &)> &readFn, const QString &typeName, const QFileSelector *selector) const
{
	testXmlValid(reader);
	if(reader.name() != QStringLiteral("Include"))
		throwElement(reader, "Include");

	auto optional = reader.boolValue("optional");
	auto fileName = reader.readElementText();
	testXmlValid(reader);
	if(reader.readNextStartElement())
		throwXmlError(reader, "The <Include> Element must not have any child elements");

	try {
		auto dir = QDir::current();
		auto dev = qobject_cast<QFileDevice*>(reader.device());
		if(dev)
			dir = QFileInfo(dev->fileName()).dir();

		QFile incFile(QDir::cleanPath(selector->select(dir.absoluteFilePath(fileName))));
		if(!incFile.open(QIODevice::ReadOnly | QIODevice::Text))
			throw SettingsXmlException(incFile);

		QXmlStreamReader incReader(&incFile);
		if(!incReader.readNextStartElement() || incReader.name() != typeName)
			throwXmlError(reader, QStringLiteral("Expected element of type <%1>").arg(typeName).toUtf8());
		readFn(incReader);
		testXmlValid(reader);
		incFile.close();

		return true;
	} catch(SettingsXmlException &e) {
		if(optional) {
			logInfo() << "Skipped include file:" << e.what();
			return false;
		} else
			throw;
	}
}

void SettingsSetupLoader::clearSetup(Setup &setup, const QString &frontend, const QStringList &selectors) const
{
	for(auto it = setup.categories.begin(); it != setup.categories.end();) {
		if(isUsable(*it, frontend, selectors)) {
			clearCategory(*it, frontend, selectors);
			it++;
		} else
			it = setup.categories.erase(it);
	}
}

void SettingsSetupLoader::clearCategory(Category &category, const QString &frontend, const QStringList &selectors) const
{
	for(auto it = category.sections.begin(); it != category.sections.end();) {
		if(isUsable(*it, frontend, selectors)) {
			clearSection(*it, frontend, selectors);
			it++;
		} else
			it = category.sections.erase(it);
	}
}

void SettingsSetupLoader::clearSection(Section &section, const QString &frontend, const QStringList &selectors) const
{
	for(auto it = section.groups.begin(); it != section.groups.end();) {
		if(isUsable(*it, frontend, selectors)) {
			clearGroup(*it, frontend, selectors);
			it++;
		} else
			it = section.groups.erase(it);
	}
}

void SettingsSetupLoader::clearGroup(Group &group, const QString &frontend, const QStringList &selectors) const
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
	_what(QStringLiteral("XML Error in file \"%1\", line %2, column %3. Error: %4")
		  .arg([&](){
			  auto dev = qobject_cast<QFileDevice*>(reader.device());
			  return dev ? dev->fileName() : QStringLiteral("<unknown>");
		  }())
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

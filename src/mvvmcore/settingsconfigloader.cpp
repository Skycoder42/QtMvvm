#include "settingsconfigloader_p.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QFileSelector>
using namespace QtMvvm;
using namespace QtMvvm::SettingsElements;

SettingsConfigLoader::SettingsConfigLoader(QObject *parent) :
	QObject{parent},
	_defaultIcon{QStringLiteral("qrc:/de/skycoder42/qtmvvm/icons/settings.svg")}
{}

void SettingsConfigLoader::changeDefaultIcon(const QUrl &defaultIcon)
{
	_defaultIcon = defaultIcon;
}

Setup SettingsConfigLoader::loadSetup(const QString &filePath, const QString &frontend, const QFileSelector *selector) const
{
	auto keyTuple = std::make_tuple(filePath, frontend, selector->allSelectors());
	Setup setup;
	if(!_cache.contains(keyTuple)) {
		try {
			const_cast<SettingsConfigLoader*>(this)->setFilters(frontend, selector);
			auto config = const_cast<SettingsConfigLoader*>(this)->readDocument(filePath);
			const_cast<SettingsConfigLoader*>(this)->resetFilters();

			if(!nonstd::holds_alternative<SettingsConfigType>(config))
				throw SettingsConfigException{"Root Element of \"" + filePath.toUtf8() + "\" must be a SettingsConfig"};

			setup = convertSettings(nonstd::get<SettingsConfigType>(config));
			_cache.insert(keyTuple, new Setup{setup});
		} catch (Exception &e) {
			throw SettingsConfigException{e};
		}
	} else
		setup = *(_cache.object(keyTuple));

	return setup;
}

QStringList QtMvvm::SettingsConfigLoader::allSelectors() const
{
	return selector()->allSelectors();
}

QString QtMvvm::SettingsConfigLoader::select(const QString &path) const
{
	return selector()->select(path);
}

Setup SettingsConfigLoader::convertSettings(const SettingsConfigType &settings) const
{
	Setup setup;

	setup.allowSearch = settings.allowSearch;
	setup.allowRestore = settings.allowRestore;

	for(const auto &element : settings.content) {
		if(nonstd::holds_alternative<CategoryType>(element)) {//holds categories -> read them
			const auto &category = nonstd::get<CategoryType>(element);
			setup.categories.append(convertCategory(category, category.content));
		} else { //hold anything else -> create default from all child and thus break
			setup.categories.append(convertCategory({}, settings.content));
			break;
		}
	}

	return setup;
}

template<typename... TContent>
Category SettingsConfigLoader::convertCategory(const CategoryType &category, const QList<variant<TContent...>> &content) const
{
	Category cat;
	cat.title = trIf(category.title, tr("General Settings"));
	cat.icon = category.icon ? QUrl{category.icon.value()} : _defaultIcon;
	cat.tooltip = trIf(category.tooltip);
	for(const auto &element : content) {
		if(nonstd::holds_alternative<SectionType>(element)) {//holds sections -> read them
			const auto &section = nonstd::get<SectionType>(element);
			cat.sections.append(convertSection(section, section.content));
		} else { //hold anything else -> create default from all child and thus break
			cat.sections.append(convertSection({}, content));
			break;
		}
	}
	return cat;
}

template<typename... TContent>
Section SettingsConfigLoader::convertSection(const SectionType &section, const QList<variant<TContent...>> &content) const
{
	Section sec;
	sec.title = trIf(section.title, tr("General"));
	sec.icon = section.icon ? QUrl{section.icon.value()} : QUrl{};
	sec.tooltip = trIf(section.tooltip);
	for(const auto &element : content) {
		if(nonstd::holds_alternative<GroupType>(element)) {//holds sections -> read them
			const auto &group = nonstd::get<GroupType>(element);
			sec.groups.append(convertGroup(group, group.content));
		} else { //hold anything else -> create default from all child and thus break
			sec.groups.append(convertGroup({}, content));
			break;
		}
	}
	return sec;
}

template<typename... TContent>
Group SettingsConfigLoader::convertGroup(const GroupType &group, const QList<variant<TContent...>> &content) const
{
	Group grp;
	grp.title = trIf(group.title);
	grp.tooltip = trIf(group.tooltip);
	for(const auto &element : content) {
		if(nonstd::holds_alternative<EntryType>(element)) //holds sections -> read them
			grp.entries.append(convertEntry(nonstd::get<EntryType>(element)));
		else
			Q_UNREACHABLE();
	}
	return grp;
}

Entry SettingsConfigLoader::convertEntry(const EntryType &entry) const
{
	Entry ent;
	ent.key = entry.key;
	ent.type = entry.type.toUtf8();
	ent.title = trIf(entry.title);
	ent.tooltip = trIf(entry.tooltip);
	ent.defaultValue = trIf<QVariant>(entry.defaultValue, entry.trdefault);
	ent.searchKeys.reserve(entry.searchKeys.size());
	for(const auto &key : entry.searchKeys)
		ent.searchKeys.append(trIf(key));
	for(const auto &prop : entry.properties) {
		auto data = readProperty(prop);
		ent.properties.insert(data.first, data.second);
	}
	return ent;
}

QVariant SettingsConfigLoader::readElement(const ElementType &element) const
{
	static const QHash<QString, QString> knownTypeMap {
		{QStringLiteral("switch"), QStringLiteral("bool")},
		{QStringLiteral("string"), QStringLiteral("QString")},
		{QStringLiteral("regexp"), QStringLiteral("QRegularExpression")},
		{QStringLiteral("number"), QStringLiteral("double")},
		{QStringLiteral("range"), QStringLiteral("int")},
		{QStringLiteral("date"), QStringLiteral("QDateTime")},
		{QStringLiteral("color"), QStringLiteral("QColor")},
		{QStringLiteral("font"), QStringLiteral("QFont")},
		{QStringLiteral("url"), QStringLiteral("QUrl")},
		{QStringLiteral("selection"), QStringLiteral("list")},
		{QStringLiteral("radiolist"), QStringLiteral("list")}
	};
	const auto &realType = knownTypeMap.value(element.type, element.type);

	//TODO move to parser for low-level asserts and conversion (i.e. content matches type, ...)
	if(realType == QStringLiteral("object")) {
		QVariantMap obj;
		for(const auto &prop : element.properties) {
			auto data = readProperty(prop);
			obj.insert(data.first, data.second);
		}
		return obj;
	} else if(realType == QStringLiteral("list")) {
		QVariantList list;
		list.reserve(element.elements.size());
		for(const auto &elem : element.elements)
			list.append(readElement(elem));
		return list;
	} else {
		auto typeId = QMetaType::type(qUtf8Printable(realType));
		if(typeId == QMetaType::UnknownType)
			throw SettingsConfigException{"Unknown typename: " + realType.toUtf8()};
		auto mVariant = trIf<QVariant>(element.content, element.tr);
		if(!mVariant.convert(typeId))
			throw SettingsConfigException("Failed to convert element data to type: " + realType.toUtf8());
		return mVariant;
	}
}

QPair<QString, QVariant> SettingsConfigLoader::readProperty(const PropertyType &property) const
{
	return {property.key, readElement(property)};
}

template <typename TType>
TType SettingsConfigLoader::trIf(const optional<QString> &text, const TType &defaultValue) const
{
	if(text)
		return TType{QCoreApplication::translate("qtmvvm_settings_xml", qUtf8Printable(text.value()))};
	else
		return defaultValue;
}

template <typename TType>
TType SettingsConfigLoader::trIf(const optional<QString> &text, bool allowTr, const TType &defaultValue) const
{
	if(allowTr)
		return trIf(text, defaultValue);
	else if(text)
		return TType{text.value()};
	else
		return defaultValue;
}



SettingsConfigException::SettingsConfigException(SettingsConfigBase::Exception &exception) :
	_what{exception.qWhat().toUtf8()}
{}

SettingsConfigException::SettingsConfigException(QByteArray what) :
	_what{std::move(what)}
{}

const char *SettingsConfigException::what() const noexcept
{
	return _what.constData();
}

void SettingsConfigException::raise() const
{
	throw *this;
}

QTMVVM_EXCEPTION_BASE *SettingsConfigException::clone() const
{
	return new SettingsConfigException{_what};
}



uint std::qHash(const std::tuple<QString, QString, QStringList> &key, uint seed)
{
	return qHash(std::get<0>(key), seed) ^
			qHash(std::get<1>(key), seed) ^
			qHash(std::get<2>(key), seed);
}

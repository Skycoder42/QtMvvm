#include "settingsconfigloader_p.h"
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
	Setup setup;
	if(!_cache.contains(filePath)) {
		try {
			auto config = const_cast<SettingsConfigLoader*>(this)->readDocument(filePath);
			if(!nonstd::holds_alternative<SettingsConfigType>(config))
				throw SettingsConfigException{"Root Element of \"" + filePath.toUtf8() + "\" must be a SettingsConfig"};
			_cache.insert(filePath, convertSettings(nonstd::get<SettingsConfigType>(config)));
		} catch (Exception &e) {
			throw SettingsConfigException{e};
		}
	} else
		setup = *(_cache.object(filePath));

	//TODO clearSetup(setup, frontend, selector->allSelectors());
	return setup;
}

Setup *SettingsConfigLoader::convertSettings(const SettingsConfigType &settings) const
{
	QScopedPointer<Setup> setup{new Setup{}};

	setup->allowSearch = settings.allowSearch;
	setup->allowRestore = settings.allowRestore;

	for(const auto &element : settings.content) {
		if(nonstd::holds_alternative<CategoryType>(element)) {//holds categories -> read them
			const auto &category = nonstd::get<CategoryType>(element);
			setup->categories.append(convertCategory(category, category.content));
		} else { //hold anything else -> create default from all child and thus break
			setup->categories.append(convertCategory({}, settings.content));
			break;
		}
	}

	return setup.take();
}

template<typename... TContent>
Category SettingsConfigLoader::convertCategory(const CategoryType &category, const QList<variant<TContent...>> &content) const
{
	Category cat;
	cat.title = category.title.value_or(tr("General Settings"));
	cat.icon = category.icon ? QUrl{category.icon.value()} : _defaultIcon;
	cat.tooltip = category.tooltip.value_or(QString{});
	cat.selectors = category.selectors.value_or(QString{});
	cat.frontends = category.frontends.value_or(QString{});
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
	sec.title = section.title.value_or(tr("General"));
	sec.icon = section.icon ? QUrl{section.icon.value()} : QUrl{};
	sec.tooltip = section.tooltip.value_or(QString{});
	sec.selectors = section.selectors.value_or(QString{});
	sec.frontends = section.frontends.value_or(QString{});
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
	grp.title = group.title.value_or(QString{});
	grp.tooltip = group.tooltip.value_or(QString{});
	grp.selectors = group.selectors.value_or(QString{});
	grp.frontends = group.frontends.value_or(QString{});
	for(const auto &element : content) {
		if(nonstd::holds_alternative<EntryType>(element)) {//holds sections -> read them
			const auto &entry = nonstd::get<EntryType>(element);
			//TODO read it
		} else
			Q_UNREACHABLE();
	}
	return grp;
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

QException *SettingsConfigException::clone() const
{
	return new SettingsConfigException{_what};
}

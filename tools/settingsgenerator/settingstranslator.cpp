#include "settingstranslator.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QDebug>

namespace {

using SourceType = SettingsConfigBase::variant<SettingsConfigBase::SettingsConfigType, SettingsConfigBase::CategoryType, SettingsConfigBase::SectionType, SettingsConfigBase::GroupType, SettingsConfigBase::EntryType>;

template <typename TVariantTarget>
void convertElement(QXmlStreamReader &reader, TVariantTarget &, SourceType &&)
{
	throw SettingsConfigBase::XmlException{reader, QStringLiteral("Unexpected root element in included file")};
}

template <typename TVariantTarget, typename TType, typename... TArgs>
void convertElement(QXmlStreamReader &reader, TVariantTarget &target, SourceType &&source)
{
	if(nonstd::holds_alternative<TType>(source))
		target = nonstd::get<TType>(std::move(source));
	else
		convertElement<TVariantTarget, TArgs...>(reader, target, std::move(source));
}

template <typename T>
struct VariantInfo {};

template <typename... TArgs>
struct VariantInfo<QList<SettingsConfigBase::variant<SettingsConfigBase::IncludeType, TArgs...>>>
{
	using TargetType = SettingsConfigBase::variant<SettingsConfigBase::IncludeType, TArgs...>;
	static void convert(QXmlStreamReader &reader, TargetType &target, SourceType &&source) {
		convertElement<TargetType, TArgs...>(reader, target, std::move(source));
	}
};

}

bool SettingsTranslator::finish_group_content(QXmlStreamReader &reader, GroupContentGroup &data, bool hasNext)
{
	hasNext = read_GroupContentGroup(reader, data, hasNext);
	finishContents(reader, data.content);
	return hasNext;
}

bool SettingsTranslator::finish_section_content(QXmlStreamReader &reader, SectionContentGroup &data, bool hasNext)
{
	hasNext = read_SectionContentGroup(reader, data, hasNext);
	finishContents(reader, data.content);
	return hasNext;
}

bool SettingsTranslator::finish_category_content(QXmlStreamReader &reader, CategoryContentGroup &data, bool hasNext)
{
	hasNext = read_CategoryContentGroup(reader, data, hasNext);
	finishContents(reader, data.content);
	return hasNext;
}

bool SettingsTranslator::finish_settings_config_content(QXmlStreamReader &reader, SettingsConfigContentGroup &data, bool hasNext)
{
	hasNext = read_SettingsConfigContentGroup(reader, data, hasNext);
	finishContents(reader, data.content);
	return hasNext;
}

template<typename TGroup>
void SettingsTranslator::finishContents(QXmlStreamReader &reader, TGroup &group)
{
	optional<std::size_t> index;
	for(auto it = group.begin(); it != group.end();) {
		// convert includes to the actual data
		if(nonstd::holds_alternative<IncludeType>(*it)) {
			if(!readGeneralInclude(reader, nonstd::get<IncludeType>(*it), it, group))
				continue;
		}
		// verify that the contents are all of the same type
		if(index) {
			if(index.value() != it->index())
				throw XmlException{reader, QStringLiteral("Detected mixture of different child elements. Only includes and a single other type are allowed")};
		} else
			index = it->index();
		++it;
	}
}

template<typename TIter, typename TList>
bool SettingsTranslator::readGeneralInclude(QXmlStreamReader &reader, IncludeType include, TIter &it, TList &list)
{
	try {
		//make the path relative if possbile
		if(dynamic_cast<QFileDevice*>(reader.device())) {
			QFileInfo docInfo{static_cast<QFileDevice*>(reader.device())->fileName()};
			include.includePath = docInfo.dir().absoluteFilePath(include.includePath);
		}
		// read the document
		VariantInfo<TList>::convert(reader, *it, readDocument(include.includePath));
		return true;
	} catch(FileException &e) {
		if(include.optional) {
			qWarning() << e.what();
			it = list.erase(it);
			return false;
		} else
			throw;
	}
}

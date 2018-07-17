#ifndef SETTINGSCONFIGIMPL_H
#define SETTINGSCONFIGIMPL_H

#include "settingsconfig.h"

class SettingsConfigImpl : public SettingsConfigBase
{
protected:
	bool finish_group_content(QXmlStreamReader &reader, GroupContentGroup &data, bool hasNext) override;
	bool finish_section_content(QXmlStreamReader &reader, SectionContentGroup &data, bool hasNext) override;
	bool finish_category_content(QXmlStreamReader &reader, CategoryContentGroup &data, bool hasNext) override;
	bool finish_settings_config_content(QXmlStreamReader &reader, SettingsConfigContentGroup &data, bool hasNext) override;

private:
	template <typename TGroup>
	void finishContents(QXmlStreamReader &reader, TGroup &group);
	template <typename TIter, typename TList>
	bool readGeneralInclude(QXmlStreamReader &reader, IncludeType include, TIter &it, TList &list);
};

#endif // SETTINGSCONFIGIMPL_H

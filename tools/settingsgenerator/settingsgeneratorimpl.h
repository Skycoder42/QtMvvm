#ifndef SETTINGSGENERATOR_H
#define SETTINGSGENERATOR_H

#include <QFile>

#include "qsettingsgenerator.h"
#include "settingsconfigimpl_p.h"

class SettingsGeneratorImpl : public SettingsGeneratorBase
{
protected:
	bool read_type_mapping(QXmlStreamReader &reader, QHash<QString, QString> &data, bool hasNext) override;
	void read_included_file(QXmlStreamReader &reader, NodeContentGroup &data) override;

	void convertFromConf(QXmlStreamReader &reader, SettingsConfigBase::SettingsConfigType &conf, SettingsType &data);
	void readCategory(QXmlStreamReader &reader, SettingsConfigBase::CategoryContentGroup &content, NodeContentGroup &targetRootNode);
	void readSection(QXmlStreamReader &reader, SettingsConfigBase::SectionContentGroup &content, NodeContentGroup &targetRootNode);
	void readGroup(QXmlStreamReader &reader, SettingsConfigBase::GroupContentGroup &content, NodeContentGroup &targetRootNode);
	void readEntry(QXmlStreamReader &reader, SettingsConfigBase::EntryType &entry, NodeContentGroup &targetRootNode);

	NodeContentGroup *findContentGroup(NodeContentGroup *cGrp, const QString &key, bool *isEntry = nullptr);
	NodeContentGroup *replaceNodeByEntry(NodeContentGroup *cGrp, NodeContentGroup *node, EntryType &entry);

	void fixTrContext(NodeContentGroup &group, const QString &context);
};

#endif // SETTINGSGENERATOR_H

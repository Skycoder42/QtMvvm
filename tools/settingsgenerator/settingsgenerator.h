#ifndef SETTINGSGENERATOR_H
#define SETTINGSGENERATOR_H

#include <QFile>
#include <QTextStream>

#include "qsettingsgenerator.h"
#include "settingstranslator.h"

class SettingsGenerator : public SettingsGeneratorBase
{
public:
	SettingsGenerator(const QString &hdrPath,
					  const QString &srcPath);

	void process(const QString &inPath);

protected:
	bool read_type_mapping(QXmlStreamReader &reader, QHash<QString, QString> &data, bool hasNext) override;
	void read_included_file(QXmlStreamReader &reader, NodeContentGroup &data) override;

private:
	QFile _hdrFile;
	QFile _srcFile;

	QTextStream _hdr;
	QTextStream _src;

	void convertFromConf(QXmlStreamReader &reader, SettingsConfigBase::SettingsConfigType &conf, SettingsType &data);
	void readCategory(QXmlStreamReader &reader, SettingsConfigBase::CategoryContentGroup &content, NodeContentGroup &targetRootNode);
	void readSection(QXmlStreamReader &reader, SettingsConfigBase::SectionContentGroup &content, NodeContentGroup &targetRootNode);
	void readGroup(QXmlStreamReader &reader, SettingsConfigBase::GroupContentGroup &content, NodeContentGroup &targetRootNode);
	void readEntry(QXmlStreamReader &reader, SettingsConfigBase::EntryType &entry, NodeContentGroup &targetRootNode);

	NodeContentGroup *findContentGroup(NodeContentGroup *cGrp, const QString &key, bool *isEntry = nullptr);
	NodeContentGroup *replaceNodeByEntry(NodeContentGroup *cGrp, NodeContentGroup *node, EntryType &&entry);

	void writeHeader(const SettingsType &settings);
	void writeNodeElements(const NodeContentGroup &node, int intendent = 1);
	void writeNode(const NodeType &node, int intendent = 1);
	void writeEntry(const EntryType &entry, int intendent = 1);

	void writeSource(const SettingsType &settings);
};

#endif // SETTINGSGENERATOR_H

#ifndef SETTINGSGENERATOR_H
#define SETTINGSGENERATOR_H

#include <QFile>
#include <QTextStream>

#include "qsettingsgenerator.h"

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

	NodeContentGroup *findContentGroup(NodeContentGroup *cGrp, const QString &key);

	void writeHeader(const SettingsType &settings);
	void writeNodeElements(const NodeContentGroup &node, int intendent = 1);
	void writeNode(const NodeType &node, int intendent = 1);
	void writeEntry(const EntryType &entry, int intendent = 1);

	void writeSource(const SettingsType &settings);
};

#endif // SETTINGSGENERATOR_H

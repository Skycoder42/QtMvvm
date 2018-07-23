#ifndef QMLSETTINGSGENERATOR_H
#define QMLSETTINGSGENERATOR_H

#include "settingsgeneratorimpl.h"
#include <tuple>

class QmlSettingsGenerator : public SettingsGeneratorImpl
{
public:
	QmlSettingsGenerator(const QString &hdrPath,
						 const QString &srcPath);

	void process(const QString &inPath);

private:
	QFile _hdrFile;
	QFile _srcFile;

	QTextStream _hdr;
	QTextStream _src;

	QString _name;
	QString _prefixName;
	QHash<QString, QString> _typeMappings;

	void writeHeader(const SettingsType &settings, const QString &inHdrPath);

	void writeListTypeClasses(const SettingsType &settings);
	std::tuple<int, QList<int>> writeNodeContentClassesDeclarations(const NodeContentGroup &node, int offset = 0);
	void writeNodeClassPropertiesDeclarations(const NodeContentGroup &node, QList<int> &childOffsets);
	void writeNodePropertyDeclaration(const NodeType &entry, int classIndex, const QString &overwriteName = {});
	void writeEntryPropertyDeclaration(const EntryType &entry, int classIndex);
	int writeNodeClassDeclaration(const NodeType &node, int offset);

	void writeSource(const SettingsType &settings);
};

#endif // QMLSETTINGSGENERATOR_H

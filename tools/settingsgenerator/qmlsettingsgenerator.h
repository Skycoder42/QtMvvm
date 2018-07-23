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

	QString _cppName;
	QString _name;
	QString _prefixName;
	QHash<QString, QString> _typeMappings;

	void writeHeader(const SettingsType &settings, const QString &inHdrPath);

	void writeListTypeBaseClass(const SettingsType &settings);

	std::tuple<int, QList<int>> writeNodeContentClassesDeclarations(const NodeContentGroup &node, const QStringList &keyList, int offset = 0);
	int writeNodeClassDeclaration(const NodeType &node, const QStringList &keyList, int offset);
	int writeListEntryListClass(const ListEntryType &entry, QStringList keyList, int offset);

	void writeNodeClassPropertiesDeclarations(const NodeContentGroup &node, const QStringList &keyList, QList<int> &childOffsets, QList<QPair<QString, int>> &childConstructs);
	void writeNodePropertyDeclaration(const NodeType &entry, int classIndex, QList<QPair<QString, int>> &childConstructs, const QString &overwriteName = {});
	void writeEntryPropertyDeclaration(const EntryType &entry, QStringList keyList, int classIndex, QList<QPair<QString, int>> &childConstructs);

	void writeSource(const SettingsType &settings);
};

#endif // QMLSETTINGSGENERATOR_H

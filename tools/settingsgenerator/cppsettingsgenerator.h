#ifndef CPPSETTINGSGENERATOR_H
#define CPPSETTINGSGENERATOR_H

#include "settingsgeneratorimpl.h"

class CppSettingsGenerator : public SettingsGeneratorImpl
{
public:
	CppSettingsGenerator(const QString &hdrPath,
						 const QString &srcPath);

	void process(const QString &inPath);

private:
	QFile _hdrFile;
	QFile _srcFile;

	QTextStream _hdr;
	QTextStream _src;

	void writeHeader(const SettingsType &settings);
	void writeNodeElementDeclarations(const NodeContentGroup &node, const QHash<QString, QString> &typeMappings, int intendent = 1);
	void writeNodeDeclaration(const NodeType &node, const QHash<QString, QString> &typeMappings, int intendent = 1);
	void writeEntryDeclaration(const EntryType &entry, const QHash<QString, QString> &typeMappings, int intendent = 1);
	void writeListEntryDeclaration(const ListEntryType &entry, const QHash<QString, QString> &typeMappings, int intendent = 1);

	void writeSource(const SettingsType &settings);
	void writeNodeElementDefinitions(const NodeContentGroup &node, const QHash<QString, QString> &typeMappings, const optional<QString> &baseKey, const QStringList &keyChain = {});
	void writeEntryDefinition(const EntryType &entry, const QHash<QString, QString> &typeMappings, const optional<QString> &baseKey, QStringList keyChain, bool skipChildren = false);
	void writeListEntryDefinition(const ListEntryType &entry, const QHash<QString, QString> &typeMappings, const optional<QString> &baseKey, QStringList keyChain);
};

#endif // CPPSETTINGSGENERATOR_H

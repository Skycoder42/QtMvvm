#ifndef CPPSETTINGSGENERATOR_H
#define CPPSETTINGSGENERATOR_H

#include "settingsgeneratorimpl.h"
#include <QTextStream>

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

	QHash<QString, QString> _typeMappings;

	void writeHeader(const SettingsType &settings);
	void writeNodeElementDeclarations(const NodeContentGroup &node, int intendent = 1);
	void writeNodeDeclaration(const NodeType &node, int intendent = 1);
	void writeEntryDeclaration(const EntryType &entry, int intendent = 1);
	void writeListNodeDeclaration(const ListNodeType &node, int intendent = 1);

	void writeSource(const SettingsType &settings);
	void writeNodeElementDefinitions(const NodeContentGroup &node,
									 const QStringList &keyChain,
									 const QStringList &entryChain = {},
									 int intendent = 1,
									 const QStringList &listTypeChain = {});
	void writeEntryDefinition(const EntryType &entry, QStringList keyChain, QStringList entryChain, int intendent, const QStringList &listTypeChain);
	void writeListNodeDefinition(const ListNodeType &node, QStringList keyChain, QStringList entryChain, int intendent, QStringList listTypeChain);

	QString concatKeys(const QStringList &keyChain, int intendet) const;
};

#endif // CPPSETTINGSGENERATOR_H

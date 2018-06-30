#ifndef SETTINGSGENERATOR_H
#define SETTINGSGENERATOR_H

#include <QFile>
#include <QXmlStreamReader>
#include <QTextStream>
#include <QSharedPointer>

class SettingsGenerator
{
public:
	SettingsGenerator(const QString &inPath,
					  const QString &hdrPath,
					  const QString &srcPath);

	void process();

private:
	QFile _inFile;
	QFile _hdrFile;
	QFile _srcFile;

	QXmlStreamReader _xml;
	QTextStream _hdr;
	QTextStream _src;

	struct Include{
		bool local;
		QString include;
	};

	struct Param {
		QString key;
		QString type;
		bool asStr;
		QString value;
	};

	struct Backend {
		QString className;
		QList<Param> params;
	};

	struct Entry;
	struct Import;
	struct Node {
		QString key;
		QList<Node> subNodes;
		QList<Entry> subEntries;
		QList<Import> subImports;
	};

	struct Entry : public Node {
		QString type;
		QString qmlGroupKey;
		QString defaultValue;
		bool defaultIsCode = false;
		bool tr;
		QString trContext;
	};

	struct Import : public Node {
		inline Import() = default;
		inline Import(Node &&node) :
			Node(std::move(node))
		{}
	};

	struct Settings : public Node {
		QString name;
		QString prefix;
		QString baseKey;

		QList<Include> includes;
		QSharedPointer<Backend> backend;
		QHash<QString, QString> typeMapping;
	};

	Include readInclude(QXmlStreamReader &xml);
	Param readParam(QXmlStreamReader &xml);
	QSharedPointer<Backend> readBackend(QXmlStreamReader &xml);
	QPair<QString, QString> readTypeMapping(QXmlStreamReader &xml);

	Node readNode(QXmlStreamReader &xml);
	Entry readEntry(QXmlStreamReader &xml);
	Import readImport(QXmlStreamReader &xml);

	Settings readSettings(QXmlStreamReader &xml);

	void writeHeader(const Settings &settings);
	void writeNodeElements(const Node &node, int intendent = 1);
	void writeNode(const Node &node, int intendent = 1);
	void writeEntry(const Entry &entry, int intendent = 1);

	void writeSource(const Settings &settings);

	template <typename T = QString>
	T readAttrib(QXmlStreamReader &xml, const QString &key, const T &defaultValue = {}, bool required = false) const;

	void checkError(QXmlStreamReader &xml) const;
	Q_NORETURN void throwFile(const QFileDevice &file) const;
	Q_NORETURN void throwReader(QXmlStreamReader &xml, const QString &overwriteError = {}) const;
	Q_NORETURN void throwChild(QXmlStreamReader &xml);
};

template<>
bool SettingsGenerator::readAttrib<bool>(QXmlStreamReader &xml, const QString &key, const bool &defaultValue, bool required) const;

#endif // SETTINGSGENERATOR_H

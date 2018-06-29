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

	struct Import {
		bool required;
		QString rootNode;
		QString import;
	};

	struct Settings : public Node {
		QString name;
		QString prefix;

		QList<Include> includes;
		QSharedPointer<Backend> backend;
		QHash<QString, QString> typeMapping;
	} _data;

	void readData();

	Include readInclude();
	Param readParam();
	QSharedPointer<Backend> readBackend();
	QPair<QString, QString> readTypeMapping();

	Node readNode();
	Entry readEntry();
	Import readImport();

	Settings readSettings();

	template <typename T = QString>
	T readAttrib(const QString &key, const T &defaultValue = {}, bool required = false) const;

	void checkError() const;
	Q_NORETURN void throwFile(const QFileDevice &file) const;
	Q_NORETURN void throwReader(const QString &overwriteError = {}) const;
	Q_NORETURN void throwChild();
};

template<>
bool SettingsGenerator::readAttrib<bool>(const QString &key, const bool &defaultValue, bool required) const;

#endif // SETTINGSGENERATOR_H

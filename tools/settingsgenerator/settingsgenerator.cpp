#include "settingsgenerator.h"
#include <QDebug>
#include <QFileInfo>
#include <QDir>

#define TABS QString{QLatin1Char('\t')}.repeated(intendent)

SettingsGenerator::SettingsGenerator(const QString &inPath, const QString &hdrPath, const QString &srcPath) :
	_inFile{inPath},
	_hdrFile{hdrPath},
	_srcFile{srcPath},
	_xml{&_inFile},
	_hdr{&_hdrFile},
	_src{&_srcFile}
{}

void SettingsGenerator::process()
{
	if(!_inFile.open(QIODevice::ReadOnly | QIODevice::Text))
		throwFile(_inFile);
	if(!_hdrFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throwFile(_hdrFile);
	if(!_srcFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throwFile(_srcFile);

	if(!_xml.readNextStartElement())
		throwReader(_xml);
	if(_xml.name() != QStringLiteral("Settings"))
		throwChild(_xml);
	auto data = readSettings(_xml);
	checkError(_xml);
	qDebug() << "Completed parsing xml - it is valid";

	data.includes.append({
							{false, QStringLiteral("QtCore/QObject")},
							{false, QStringLiteral("QtMvvmCore/ISettingsAccessor")},
							{false, QStringLiteral("QtMvvmCore/SettingsEntry")},
						 });

	writeHeader(data);
	writeSource(data);
}

SettingsGenerator::Include SettingsGenerator::readInclude(QXmlStreamReader &xml)
{
	Include include;
	include.local = readAttrib<bool>(xml, QStringLiteral("local"), false);
	include.include = xml.readElementText();
	checkError(xml);
	return include;
}

SettingsGenerator::Param SettingsGenerator::readParam(QXmlStreamReader &xml)
{
	Param param;
	param.key = readAttrib(xml, QStringLiteral("key"), {}, true);
	param.type = readAttrib(xml, QStringLiteral("type"), {}, true);
	param.asStr = readAttrib<bool>(xml, QStringLiteral("asStr"), false);
	param.value = xml.readElementText();
	checkError(xml);
	return param;
}

QSharedPointer<SettingsGenerator::Backend> SettingsGenerator::readBackend(QXmlStreamReader &xml)
{
	auto backend = QSharedPointer<Backend>::create();
	backend->className = readAttrib(xml, QStringLiteral("class"), {}, true);
	while(xml.readNextStartElement()) {
		if(xml.name() == QStringLiteral("Param"))
			backend->params.append(readParam(xml));
		else
			throwChild(xml);
	}
	checkError(xml);
	return backend;
}

QPair<QString, QString> SettingsGenerator::readTypeMapping(QXmlStreamReader &xml)
{
	QPair<QString, QString> mapping;
	mapping.first = readAttrib(xml, QStringLiteral("key"), {}, true);
	mapping.second = readAttrib(xml, QStringLiteral("type"), {}, true);
	checkError(xml);
	return mapping;
}

SettingsGenerator::Node SettingsGenerator::readNode(QXmlStreamReader &xml)
{
	Node node;
	node.key = readAttrib(xml, QStringLiteral("key"), {}, true);
	while(xml.readNextStartElement()) {
		if(xml.name() == QStringLiteral("Node"))
			node.subNodes.append(readNode(xml));
		else if(xml.name() == QStringLiteral("Entry"))
			node.subEntries.append(readEntry(xml));
		else if(xml.name() == QStringLiteral("Import"))
			node.subImports.append(readImport(xml));
		else
			throwChild(xml);
	}
	checkError(xml);
	return node;
}

SettingsGenerator::Entry SettingsGenerator::readEntry(QXmlStreamReader &xml)
{
	Entry entry;
	entry.key = readAttrib(xml, QStringLiteral("key"), {}, true);
	entry.type = readAttrib(xml, QStringLiteral("type"), {}, true);
	entry.qmlGroupKey = readAttrib(xml, QStringLiteral("qmlGroupKey"));
	entry.defaultValue = readAttrib(xml, QStringLiteral("default"));
	entry.tr = readAttrib<bool>(xml, QStringLiteral("tr"), false);
	entry.trContext = readAttrib(xml, QStringLiteral("trContext"));
	while(xml.readNextStartElement()) {
		if(xml.name() == QStringLiteral("Code")) {
			if(!entry.defaultValue.isNull())
				throwChild(xml);
			entry.defaultValue = xml.readElementText();
			entry.defaultIsCode = true;
			checkError(xml);
		} else if(xml.name() == QStringLiteral("Node"))
			entry.subNodes.append(readNode(xml));
		else if(xml.name() == QStringLiteral("Entry"))
			entry.subEntries.append(readEntry(xml));
		else if(xml.name() == QStringLiteral("Import"))
			entry.subImports.append(readImport(xml));
		else
			throwChild(xml);
	}
	checkError(xml);
	return entry;
}

SettingsGenerator::Import SettingsGenerator::readImport(QXmlStreamReader &xml)
{
	auto required = readAttrib<bool>(xml, QStringLiteral("required"), true);
	auto rootNode = readAttrib(xml, QStringLiteral("rootNode")).split(QLatin1Char('/'), QString::SkipEmptyParts);
	auto path = xml.readElementText();
	checkError(xml);

	auto pInfo = QFileInfo{static_cast<QFileDevice*>(xml.device())->fileName()}.dir();
	QFile subFile{pInfo.absoluteFilePath(path)};
	if(!subFile.exists()) {
		if(required)
			throwReader(xml, QStringLiteral("Failed to find required import file \"%1\"").arg(subFile.fileName()));
		else
			return {};
	}

	if(!subFile.open(QIODevice::ReadOnly | QIODevice::Text))
		throwFile(subFile);

	//TODO implement

	QXmlStreamReader reader{&subFile};
	if(!reader.readNextStartElement())
		throwReader(reader);
	if(reader.name() != QStringLiteral("Settings"))
		throwChild(reader);
	Node subSettings = readSettings(reader);
	checkError(reader);
	for(const auto &key : rootNode) {
		auto found = false;
		for(const auto &sNode : qAsConst(subSettings.subNodes)) {
			if(sNode.key == key) {
				found = true;
				subSettings = sNode;
				break;
			}
		}
		for(const auto &sNode : qAsConst(subSettings.subEntries)) {
			if(sNode.key == key) {
				found = true;
				subSettings = sNode;
				break;
			}
		}
		for(const auto &sNode : qAsConst(subSettings.subImports)) {
			if(sNode.key == key) {
				found = true;
				subSettings = sNode;
				break;
			}
		}
		if(!found)
			throwReader(xml, QStringLiteral("Unable to find rootNode \"%1\" in loaded document").arg(rootNode.join(QLatin1Char('/'))));
	}

	return std::move(subSettings);
}

SettingsGenerator::Settings SettingsGenerator::readSettings(QXmlStreamReader &xml)
{
	Settings settings;
	settings.name = readAttrib(xml, QStringLiteral("name"), {}, true);
	settings.prefix = readAttrib(xml, QStringLiteral("prefix"));

	while(xml.readNextStartElement()) {
		if(xml.name() == QStringLiteral("Include"))
			settings.includes.append(readInclude(xml));
		else if(xml.name() == QStringLiteral("Backend"))
			settings.backend = readBackend(xml);
		else if(xml.name() == QStringLiteral("TypeMapping")) {
			auto mapping = readTypeMapping(xml);
			settings.typeMapping.insert(mapping.first, mapping.second);
		} else if(xml.name() == QStringLiteral("Node"))
			settings.subNodes.append(readNode(xml));
		else if(xml.name() == QStringLiteral("Entry"))
			settings.subEntries.append(readEntry(xml));
		else if(xml.name() == QStringLiteral("Import"))
			settings.subImports.append(readImport(xml));
		else
			throwChild(xml);
	}
	checkError(xml);
	return settings;
}

void SettingsGenerator::writeHeader(const Settings &settings)
{
	QString incGuard = QFileInfo{_hdrFile.fileName()}
					   .completeBaseName()
					   .replace(QLatin1Char('.'), QLatin1Char('_'))
					   .toUpper() + QStringLiteral("_H");
	_hdr << "#ifndef " << incGuard << '\n'
		 << "#define " << incGuard << "\n\n";

	// write the includes
	for(const auto &inc : settings.includes) {
		if(inc.local)
			_hdr << "#include \"" << inc.include << "\"\n";
		else
			_hdr << "#include <" << inc.include << ">\n";
	}
	_hdr << "\n";

	// create the class
	QString pName = settings.prefix.isEmpty() ? settings.name : (settings.prefix + QLatin1Char(' ') + settings.name);
	_hdr << "class " << pName << " : public QObject\n"
		 << "{\n"
		 << "\tQ_OBJECT\n\n"
		 << "\tQ_PROPERTY(ISettingsAccessor *accessor READ accessor CONSTANT FINAL)\n\n"
		 << "public:\n"
		 << "\tQ_INVOKABLE explicit " << settings.name << "(QObject *parent = nullptr);\n"
		 << "\texplicit " << settings.name << "(QObject *parent, ISettingsAccessor *accessor);\n\n"
		 << "\tstatic " << settings.name << " *instance();\n\n"
		 << "\tISettingsAccessor *accessor() const;\n\n";

	writeNodeElements(settings);

	_hdr << "\nprivate:\n"
		 << "\tISettingsAccessor *_accessor;\n"
		 << "};\n\n"
		 << "#endif //" << incGuard << '\n';
}

void SettingsGenerator::writeNodeElements(const SettingsGenerator::Node &node, int intendent)
{
	for(const auto &cNode : node.subNodes)
		writeNode(cNode, intendent);
	for(const auto &cEntry : node.subEntries)
		writeEntry(cEntry, intendent);
}

void SettingsGenerator::writeNode(const SettingsGenerator::Node &node, int intendent)
{
	_hdr << TABS << "struct {\n";
	writeNodeElements(node, intendent + 1);
	_hdr << TABS << "} " << node.key << ";\n";
}

void SettingsGenerator::writeEntry(const SettingsGenerator::Entry &entry, int intendent)
{
	_hdr << TABS << "struct : QtMvvm::SettingsEntry<" << entry.type << "> {\n";
	writeNodeElements(entry, intendent + 1);
	_hdr << TABS << "} " << entry.key << ";\n";
}

void SettingsGenerator::writeSource(const Settings &settings)
{

}

void SettingsGenerator::checkError(QXmlStreamReader &xml) const
{
	if(xml.hasError())
		throwReader(xml);
}

template<typename T>
T SettingsGenerator::readAttrib(QXmlStreamReader &xml, const QString &key, const T &defaultValue, bool required) const
{
	if(xml.attributes().hasAttribute(key))
		return QVariant(xml.attributes().value(key).toString()).template value<T>();
	else if(required)
		throwReader(xml, QStringLiteral("Required attribute \"%1\" but was not set").arg(key));
	else
		return defaultValue;
}

template<>
bool SettingsGenerator::readAttrib<bool>(QXmlStreamReader &xml, const QString &key, const bool &defaultValue, bool required) const
{
	if(xml.attributes().hasAttribute(key)) {
		if(xml.attributes().value(key) == QStringLiteral("true"))
			return true;
		else if(xml.attributes().value(key) == QStringLiteral("false"))
			return false;
		else
			throwReader(xml, QStringLiteral("Value of attribute \"%1\" is not a xs:boolean!").arg(key));
	} else if(required)
		throwReader(xml, QStringLiteral("Required attribute \"%1\" but was not set").arg(key));
	else
		return defaultValue;
}

void SettingsGenerator::throwFile(const QFileDevice &file) const
{
	throw QStringLiteral("%1: %2").arg(file.fileName(), file.errorString());
}

void SettingsGenerator::throwReader(QXmlStreamReader &xml, const QString &overwriteError) const
{
	throw QStringLiteral("%1:%2:%3: %4")
			.arg(dynamic_cast<QFileDevice*>(xml.device())->fileName())
			.arg(xml.lineNumber())
			.arg(xml.columnNumber())
			.arg(overwriteError.isNull() ? xml.errorString() : overwriteError);
}

void SettingsGenerator::throwChild(QXmlStreamReader &xml)
{
	throwReader(xml, QStringLiteral("Unexpected child element: %1").arg(xml.name()));
}

#include "settingsgenerator.h"
#include <QDebug>

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

	readData();
}

void SettingsGenerator::readData()
{
	if(!_xml.readNextStartElement())
		throwReader();
	if(_xml.name() != QStringLiteral("Settings"))
		throwChild();

	_data = readSettings();
	checkError();
	qDebug() << "Completed parsing xml - it is valid";
}

SettingsGenerator::Include SettingsGenerator::readInclude()
{
	Include include;
	include.local = readAttrib<bool>(QStringLiteral("local"), false);
	include.include = _xml.readElementText();
	checkError();
	return include;
}

SettingsGenerator::Param SettingsGenerator::readParam()
{
	Param param;
	param.key = readAttrib(QStringLiteral("key"), {}, true);
	param.type = readAttrib(QStringLiteral("type"), {}, true);
	param.asStr = readAttrib<bool>(QStringLiteral("asStr"), false);
	param.value = _xml.readElementText();
	checkError();
	return param;
}

QSharedPointer<SettingsGenerator::Backend> SettingsGenerator::readBackend()
{
	auto backend = QSharedPointer<Backend>::create();
	backend->className = readAttrib(QStringLiteral("class"), {}, true);
	while(_xml.readNextStartElement()) {
		if(_xml.name() == QStringLiteral("Param"))
			backend->params.append(readParam());
		else
			throwChild();
	}
	checkError();
	return backend;
}

QPair<QString, QString> SettingsGenerator::readTypeMapping()
{
	QPair<QString, QString> mapping;
	mapping.first = readAttrib(QStringLiteral("key"), {}, true);
	mapping.second = readAttrib(QStringLiteral("type"), {}, true);
	checkError();
	return mapping;
}

SettingsGenerator::Node SettingsGenerator::readNode()
{
	Node node;
	node.key = readAttrib(QStringLiteral("key"), {}, true);
	while(_xml.readNextStartElement()) {
		if(_xml.name() == QStringLiteral("Node"))
			node.subNodes.append(readNode());
		else if(_xml.name() == QStringLiteral("Entry"))
			node.subEntries.append(readEntry());
		else if(_xml.name() == QStringLiteral("Import"))
			node.subImports.append(readImport());
		else
			throwChild();
	}
	checkError();
	return node;
}

SettingsGenerator::Entry SettingsGenerator::readEntry()
{
	Entry entry;
	entry.key = readAttrib(QStringLiteral("key"), {}, true);
	entry.type = readAttrib(QStringLiteral("type"), {}, true);
	entry.qmlGroupKey = readAttrib(QStringLiteral("qmlGroupKey"));
	entry.defaultValue = readAttrib(QStringLiteral("default"));
	entry.tr = readAttrib<bool>(QStringLiteral("tr"), false);
	entry.trContext = readAttrib(QStringLiteral("trContext"));
	while(_xml.readNextStartElement()) {
		if(_xml.name() == QStringLiteral("Code")) {
			if(!entry.defaultValue.isNull())
				throwChild();
			entry.defaultValue = _xml.readElementText();
			entry.defaultIsCode = true;
			checkError();
		} else if(_xml.name() == QStringLiteral("Node"))
			entry.subNodes.append(readNode());
		else if(_xml.name() == QStringLiteral("Entry"))
			entry.subEntries.append(readEntry());
		else if(_xml.name() == QStringLiteral("Import"))
			entry.subImports.append(readImport());
		else
			throwChild();
	}
	checkError();
	return entry;
}

SettingsGenerator::Import SettingsGenerator::readImport()
{
	Import import;
	import.required = readAttrib<bool>(QStringLiteral("required"), true);
	import.rootNode = readAttrib(QStringLiteral("rootNode"));
	import.import = _xml.readElementText();
	checkError();
	return import;
}

SettingsGenerator::Settings SettingsGenerator::readSettings()
{
	Settings settings;
	settings.name = readAttrib(QStringLiteral("name"), {}, true);
	settings.prefix = readAttrib(QStringLiteral("prefix"));

	while(_xml.readNextStartElement()) {
		if(_xml.name() == QStringLiteral("Include"))
			settings.includes.append(readInclude());
		else if(_xml.name() == QStringLiteral("Backend"))
			settings.backend = readBackend();
		else if(_xml.name() == QStringLiteral("TypeMapping")) {
			auto mapping = readTypeMapping();
			settings.typeMapping.insert(mapping.first, mapping.second);
		} else if(_xml.name() == QStringLiteral("Node"))
			settings.subNodes.append(readNode());
		else if(_xml.name() == QStringLiteral("Entry"))
			settings.subEntries.append(readEntry());
		else if(_xml.name() == QStringLiteral("Import"))
			settings.subImports.append(readImport());
		else
			throwChild();
	}
	checkError();
	return settings;
}

void SettingsGenerator::checkError() const
{
	if(_xml.hasError())
		throwReader();
}

template<typename T>
T SettingsGenerator::readAttrib(const QString &key, const T &defaultValue, bool required) const
{
	if(_xml.attributes().hasAttribute(key))
		return QVariant(_xml.attributes().value(key).toString()).template value<T>();
	else if(required)
		throwReader(QStringLiteral("Required attribute \"%1\" but was not set").arg(key));
	else
		return defaultValue;
}

template<>
bool SettingsGenerator::readAttrib<bool>(const QString &key, const bool &defaultValue, bool required) const
{
	if(_xml.attributes().hasAttribute(key)) {
		if(_xml.attributes().value(key) == QStringLiteral("true"))
			return true;
		else if(_xml.attributes().value(key) == QStringLiteral("false"))
			return false;
		else
			throwReader(QStringLiteral("Value of attribute \"%1\" is not a xs:boolean!").arg(key));
	} else if(required)
		throwReader(QStringLiteral("Required attribute \"%1\" but was not set").arg(key));
	else
		return defaultValue;
}

void SettingsGenerator::throwFile(const QFileDevice &file) const
{
	throw QStringLiteral("%1: %2").arg(file.fileName(), file.errorString());
}

void SettingsGenerator::throwReader(const QString &overwriteError) const
{
	throw QStringLiteral("%1:%2:%3: %4")
			.arg(dynamic_cast<QFileDevice*>(_xml.device())->fileName())
			.arg(_xml.lineNumber())
			.arg(_xml.columnNumber())
			.arg(overwriteError.isNull() ? _xml.errorString() : overwriteError);
}

void SettingsGenerator::throwChild()
{
	throwReader(QStringLiteral("Unexpected child element: %1").arg(_xml.name()));
}

#include "settingsgenerator.h"
#include <QDebug>
#include <QFileInfo>
#include <QDir>

#define TABS QString{QLatin1Char('\t')}.repeated(intendent)

SettingsGenerator::SettingsGenerator(const QString &hdrPath, const QString &srcPath) :
	_hdrFile{hdrPath},
	_srcFile{srcPath},
	_hdr{&_hdrFile},
	_src{&_srcFile}
{}

void SettingsGenerator::process(const QString &inPath)
{
	// TODO add xml patterns...
	auto settings = readDocument(inPath);

	if(!_hdrFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throw FileException{_hdrFile};
	writeHeader(settings);
	_hdrFile.close();

	if(!_srcFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throw FileException{_srcFile};
	writeSource(settings);
	_srcFile.close();
}

bool SettingsGenerator::read_type_mapping(QXmlStreamReader &reader, QHash<QString, QString> &data, bool hasNext)
{
	TypeMappingGroup grp;
	hasNext = read_TypeMappingGroup(reader, grp, hasNext);
	for(const auto &mapping : qAsConst(grp.typeMapping))
		data.insert(mapping.key, mapping.type);
	return hasNext;
}

void SettingsGenerator::read_included_file(QXmlStreamReader &reader, NodeContentGroup &data)
{
	ImportType import;
	read_ImportType(reader, import);

	//make the path relative if possbile
	if(dynamic_cast<QFileDevice*>(reader.device())) {
		QFileInfo docInfo{static_cast<QFileDevice*>(reader.device())->fileName()};
		import.importPath = docInfo.dir().absoluteFilePath(import.importPath);
	}

	// read the document
	try {
		auto settings = readDocument(import.importPath);
		NodeContentGroup *cGrp = &settings;
		if(import.rootNode) {
			for(const auto &key : import.rootNode.value().split(QLatin1Char('/'), QString::SkipEmptyParts)) {
				cGrp = findContentGroup(cGrp, key);
				if(!cGrp)
					return;
			}
		}
		data = *cGrp;
	} catch(QException &e) {
		if(import.required)
			throw;
		else {
			qWarning() << e.what();
			return;
		}
	}
}

SettingsGeneratorBase::NodeContentGroup *SettingsGenerator::findContentGroup(SettingsGeneratorBase::NodeContentGroup *cGrp, const QString &key)
{
	for(const auto &cNode : cGrp->contentNodes) {
		if(nonstd::holds_alternative<NodeType>(cNode)) {
			if(std::get<NodeType>(cNode).key == key)
				return const_cast<NodeType*>(&(std::get<NodeType>(cNode)));
		} else if(nonstd::holds_alternative<EntryType>(cNode)) {
			if(std::get<EntryType>(cNode).key == key)
				return const_cast<EntryType*>(&(std::get<EntryType>(cNode)));
		} else if(nonstd::holds_alternative<NodeContentGroup>(cNode)) {
			auto res = findContentGroup(const_cast<NodeContentGroup*>(&(std::get<NodeContentGroup>(cNode))), key);
			if(res)
				return res;
		}
	}

	return nullptr;
}

void SettingsGenerator::writeHeader(const SettingsType &settings)
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
			_hdr << "#include \"" << inc.includePath << "\"\n";
		else
			_hdr << "#include <" << inc.includePath << ">\n";
	}
	_hdr << "\n";

	// create the class
	auto name = settings.name.value_or(QFileInfo{_hdrFile.fileName()}.baseName());
	_hdr << "class " << QString{settings.prefix ? settings.prefix.value() + QLatin1Char(' ') + name : name} << " : public QObject\n"
		 << "{\n"
		 << "\tQ_OBJECT\n\n"
		 << "\tQ_PROPERTY(ISettingsAccessor *accessor READ accessor CONSTANT FINAL)\n\n"
		 << "public:\n"
		 << "\tQ_INVOKABLE explicit " << name << "(QObject *parent = nullptr);\n"
		 << "\texplicit " << name << "(QObject *parent, ISettingsAccessor *accessor);\n\n"
		 << "\tstatic " << name << " *instance();\n\n"
		 << "\tISettingsAccessor *accessor() const;\n\n";

	writeNodeElements(settings);

	_hdr << "\nprivate:\n"
		 << "\tISettingsAccessor *_accessor;\n"
		 << "};\n\n"
		 << "#endif //" << incGuard << '\n';
}

void SettingsGenerator::writeNodeElements(const NodeContentGroup &node, int intendent)
{
	for(const auto &cNode : node.contentNodes) {
		if(nonstd::holds_alternative<NodeType>(cNode))
			writeNode(nonstd::get<NodeType>(cNode), intendent);
		else if(nonstd::holds_alternative<EntryType>(cNode))
			writeEntry(nonstd::get<EntryType>(cNode), intendent);
		else if(nonstd::holds_alternative<NodeContentGroup>(cNode))
			writeNodeElements(nonstd::get<NodeContentGroup>(cNode), intendent);
	}
}

void SettingsGenerator::writeNode(const NodeType &node, int intendent)
{
	_hdr << TABS << "struct {\n";
	writeNodeElements(node, intendent + 1);
	_hdr << TABS << "} " << node.key << ";\n";
}

void SettingsGenerator::writeEntry(const EntryType &entry, int intendent)
{
	_hdr << TABS << "struct : QtMvvm::SettingsEntry<" << entry.type << "> {\n";
	writeNodeElements(entry, intendent + 1);
	_hdr << TABS << "} " << entry.key << ";\n";
}

void SettingsGenerator::writeSource(const SettingsType &settings)
{

}

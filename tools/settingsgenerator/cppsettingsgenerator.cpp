#include "cppsettingsgenerator.h"
#include <QFileInfo>

#define TABS QString{QLatin1Char('\t')}.repeated(intendent)

CppSettingsGenerator::CppSettingsGenerator(const QString &hdrPath, const QString &srcPath) :
	_hdrFile{hdrPath},
	_srcFile{srcPath},
	_hdr{&_hdrFile},
	_src{&_srcFile}
{}

void CppSettingsGenerator::process(const QString &inPath)
{
	// read settings and adjust defaults
	auto settings = readDocument(inPath);
	if(!settings.name)
		settings.name = QFileInfo{inPath}.baseName();
	fixTrContext(settings, QFileInfo{inPath}.fileName());

	_typeMappings = settings.typeMappings;

	if(!_hdrFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throw FileException{_hdrFile};
	writeHeader(settings);
	_hdr.flush();
	_hdrFile.close();

	if(!_srcFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throw FileException{_srcFile};
	writeSource(settings);
	_src.flush();
	_srcFile.close();
}

void CppSettingsGenerator::writeHeader(const SettingsType &settings)
{
	QString incGuard = QFileInfo{_hdrFile.fileName()}
					   .completeBaseName()
					   .replace(QLatin1Char('.'), QLatin1Char('_'))
					   .toUpper() + QStringLiteral("_H");
	_hdr << "#ifndef " << incGuard << '\n'
		 << "#define " << incGuard << "\n\n";

	// write the includes
	auto includes = QList<IncludeType> {
		{false, QStringLiteral("QtCore/QObject")},
		{false, QStringLiteral("QtMvvmCore/ISettingsAccessor")},
		{false, QStringLiteral("QtMvvmCore/SettingsEntry")}
	} + settings.includes;
	for(const auto &inc : includes) {
		if(inc.local)
			_hdr << "#include \"" << inc.includePath << "\"\n";
		else
			_hdr << "#include <" << inc.includePath << ">\n";
	}
	_hdr << "\n";

	// create the class
	_hdr << "class " << (settings.prefix ? settings.prefix.value() + QLatin1Char(' ') + settings.name.value() : settings.name.value()) << " : public QObject\n"
		 << "{\n"
		 << "\tQ_OBJECT\n\n"
		 << "\tQ_PROPERTY(QtMvvm::ISettingsAccessor *accessor READ accessor CONSTANT FINAL)\n\n"
		 << "public:\n"
		 << "\tQ_INVOKABLE explicit " << settings.name.value() << "(QObject *parent = nullptr);\n"
		 << "\texplicit " << settings.name.value() << "(QtMvvm::ISettingsAccessor *accessor, QObject *parent);\n\n"
		 << "\tstatic " << settings.name.value() << " *instance();\n\n"
		 << "\tQtMvvm::ISettingsAccessor *accessor() const;\n\n";

	writeNodeElementDeclarations(settings);

	_hdr << "\nprivate:\n"
		 << "\tQtMvvm::ISettingsAccessor *_accessor;\n"
		 << "};\n\n"
		 << "#endif //" << incGuard << '\n';
}

void CppSettingsGenerator::writeNodeElementDeclarations(const NodeContentGroup &node, int intendent)
{
	for(const auto &cNode : node.contentNodes) {
		if(nonstd::holds_alternative<NodeType>(cNode))
			writeNodeDeclaration(nonstd::get<NodeType>(cNode), intendent);
		else if(nonstd::holds_alternative<EntryType>(cNode))
			writeEntryDeclaration(nonstd::get<EntryType>(cNode), intendent);
		else if(nonstd::holds_alternative<ListNodeType>(cNode))
			writeListNodeDeclaration(nonstd::get<ListNodeType>(cNode), intendent);
		else if(nonstd::holds_alternative<NodeContentGroup>(cNode))
			writeNodeElementDeclarations(nonstd::get<NodeContentGroup>(cNode), intendent);
		else
			Q_UNREACHABLE();
	}
}

void CppSettingsGenerator::writeNodeDeclaration(const NodeType &node, int intendent)
{
	_hdr << TABS << "struct { //" << node.key << "\n";
	writeNodeElementDeclarations(node, intendent + 1);
	_hdr << TABS << "} " << node.key << ";\n";
}

void CppSettingsGenerator::writeEntryDeclaration(const EntryType &entry, int intendent)
{
	if(entry.contentNodes.isEmpty())
		_hdr << TABS << "QtMvvm::SettingsEntry<" << _typeMappings.value(entry.type, entry.type) << "> " << entry.key << ";\n";
	else {
		const auto &mType = _typeMappings.value(entry.type, entry.type);
		_hdr << TABS << "struct : QtMvvm::SettingsEntry<" << mType << "> { //" << entry.key << "\n";
		writeNodeElementDeclarations(entry, intendent + 1);
		_hdr << TABS << "\tinline auto &operator=(const " << mType << " &__value) { SettingsEntry<" << mType << ">::operator=(__value); return *this; }\n";
		_hdr << TABS << "} " << entry.key << ";\n";
	}
}

void CppSettingsGenerator::writeListNodeDeclaration(const ListNodeType &node, int intendent)
{
	_hdr << TABS << "struct " << node.key << "_element {\n";
	writeNodeElementDeclarations(node, intendent + 1);
	_hdr << TABS << "};\n";
	_hdr << TABS << "QtMvvm::SettingsListNode<" << node.key << "_element> " << node.key << ";\n";
}

void CppSettingsGenerator::writeSource(const SettingsType &settings)
{
	_src << "#include \"" << _hdrFile.fileName() << "\"\n";
	_src << "#include <QtCore/QCoreApplication>\n";
	_src << "#include <QtMvvmCore/ServiceRegistry>\n";
	if(!settings.backend)
		_src << "#include <QtMvvmCore/QSettingsAccessor>\n";
	_src << "\n";

	_src << "namespace {\n\n"
		 << "void __generated_settings_setup()\n"
		 << "{\n"
		 << "\tQtMvvm::ServiceRegistry::instance()->registerObject<" << settings.name.value() << ">(";
	if(settings.scope) {
		switch(settings.scope.value()) {
		case SettingsGeneratorBase::DestroyOnAppQuit:
			_src << "QtMvvm::ServiceRegistry::DestroyOnAppQuit";
			break;
		case SettingsGeneratorBase::DestroyOnAppDestroy:
			_src << "QtMvvm::ServiceRegistry::DestroyOnAppDestroy";
			break;
		case SettingsGeneratorBase::DestroyOnRegistryDestroy:
			_src << "QtMvvm::ServiceRegistry::DestroyOnRegistryDestroy";
			break;
		case SettingsGeneratorBase::DestroyNever:
			_src << "QtMvvm::ServiceRegistry::DestroyNever";
			break;
		default:
			Q_UNREACHABLE();
			break;
		}
	}
	_src << ");\n"
		 << "}\n\n"
		 << "}\n"
		 << "Q_COREAPP_STARTUP_FUNCTION(__generated_settings_setup)\n\n";

	_src << settings.name.value() << "::" << settings.name.value() << "(QObject *parent) : \n";
	if(settings.backend) {
		const auto &backend = settings.backend.value();
		_src << "\t" << settings.name.value() << "{new " << backend.className << "{";
		if(!backend.param.isEmpty()) {
			_src << "\n";
			auto first = true;
			for(const auto &param : qAsConst(backend.param)) {
				if(first)
					first = false;
				else
					_src << ",\n";
				_src << "\t\t";
				if(param.asStr)
					_src << "QVariant{QStringLiteral(\"" << param.value << "\")}.value<" << param.type << ">()";
				else
					_src << param.value;
			}
			_src << "\n\t";
		}
		_src << "}, parent}\n";
	} else
		_src << "\t" << settings.name.value() << "{QtMvvm::ISettingsAccessor::createDefaultAccessor(nullptr), parent}\n";
	_src << "{\n"
		 << "\t_accessor->setParent(this);\n"
		 << "}\n\n";

	_src << settings.name.value() << "::" << settings.name.value() << "(QtMvvm::ISettingsAccessor *accessor, QObject *parent) : \n"
		 << "\tQObject{parent},\n"
		 << "\t_accessor{accessor}\n"
		 << "{\n";
	writeNodeElementDefinitions(settings, settings.baseKey ? QStringList{settings.baseKey.value()} : QStringList{});
	_src << "}\n\n";

	_src << settings.name.value() << " *" << settings.name.value() << "::instance()\n"
		 << "{\n"
		 << "\treturn QtMvvm::ServiceRegistry::instance()->service<" << settings.name.value() << ">();\n"
		 << "}\n\n";

	_src << "QtMvvm::ISettingsAccessor *" << settings.name.value() << "::accessor() const\n"
		 << "{\n"
		 << "\treturn _accessor;\n"
		 << "}\n\n";
}

void CppSettingsGenerator::writeNodeElementDefinitions(const SettingsGeneratorBase::NodeContentGroup &node, const QStringList &keyChain, const QStringList &entryChain, int intendent, const QStringList &listTypeChain)
{
	for(const auto &cNode : node.contentNodes) {
		if(nonstd::holds_alternative<NodeType>(cNode)) {
			const auto &xNode = nonstd::get<NodeType>(cNode);
			writeNodeElementDefinitions(xNode, QStringList{keyChain} << xNode.key, QStringList{entryChain} << xNode.key, intendent, listTypeChain);
		} else if(nonstd::holds_alternative<EntryType>(cNode))
			writeEntryDefinition(nonstd::get<EntryType>(cNode), keyChain, entryChain, intendent, listTypeChain);
		else if(nonstd::holds_alternative<ListNodeType>(cNode))
			writeListNodeDefinition(nonstd::get<ListNodeType>(cNode), keyChain, entryChain, intendent, listTypeChain);
		else if(nonstd::holds_alternative<NodeContentGroup>(cNode))
			writeNodeElementDefinitions(nonstd::get<NodeContentGroup>(cNode), keyChain, entryChain, intendent, listTypeChain);
		else
			Q_UNREACHABLE();
	}
}

void CppSettingsGenerator::writeEntryDefinition(const SettingsGeneratorBase::EntryType &entry, QStringList keyChain, QStringList entryChain, int intendent, const QStringList &listTypeChain)
{
	entryChain.append(entry.key);
	keyChain.append(entry.key);
	_src << TABS << entryChain.join(QLatin1Char('.'))
		 << ".setup(" << concatKeys(keyChain, intendent) << ", _accessor";
	if(entry.code)
		_src << ", QVariant::fromValue<" << _typeMappings.value(entry.type, entry.type) << ">(" << entry.code.value().trimmed() << ")";
	else if(entry.defaultValue) {
		_src << ", QVariant{";
		if(entry.tr)
			_src << "QCoreApplication::translate(\"" << entry.trContext.value() << "\", \"" << entry.defaultValue.value() << "\")";
		else
			_src << "QStringLiteral(\"" << entry.defaultValue.value() << "\")";
		_src << "}.value<" << _typeMappings.value(entry.type, entry.type) << ">()";
	}
	_src << ");\n";
	writeNodeElementDefinitions(entry, keyChain, entryChain, intendent, listTypeChain);
}

void CppSettingsGenerator::writeListNodeDefinition(const ListNodeType &node, QStringList keyChain, QStringList entryChain, int intendent, QStringList listTypeChain)
{
	keyChain.append(node.key);
	entryChain.append(node.key);
	listTypeChain.append(node.key + QStringLiteral("_element"));
	_src << TABS << entryChain.join(QLatin1Char('.'))
		 << ".setup(" << concatKeys(keyChain, intendent) << ", _accessor, [=](int __index_" << intendent << ", " << listTypeChain.join(QStringLiteral("::")) << " &__entry) {\n";

	keyChain.append(QLatin1Char('%') + QString::number(intendent));
	entryChain = QStringList{QStringLiteral("__entry")};
	writeNodeElementDefinitions(node, keyChain, entryChain, intendent + 1, listTypeChain);
	_src << TABS << "});\n";
}

QString CppSettingsGenerator::concatKeys(const QStringList &keyChain, int intendet) const
{
	QString res = QStringLiteral("QStringLiteral(\"") + keyChain.join(QLatin1Char('/')) + QStringLiteral("\")");
	for(auto i = 1; i < intendet; i++)
		res.append(QStringLiteral(".arg(__index_%1)").arg(i));
	return res;
}


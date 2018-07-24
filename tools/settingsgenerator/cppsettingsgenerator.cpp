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

	if(!_hdrFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throw FileException{_hdrFile};
	writeHeader(settings);
	_hdrFile.close();

	if(!_srcFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throw FileException{_srcFile};
	writeSource(settings);
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

	writeNodeElementDeclarations(settings, settings.typeMappings);

	_hdr << "\nprivate:\n"
		 << "\tQtMvvm::ISettingsAccessor *_accessor;\n"
		 << "};\n\n"
		 << "#endif //" << incGuard << '\n';
}

void CppSettingsGenerator::writeNodeElementDeclarations(const NodeContentGroup &node, const QHash<QString, QString> &typeMappings, int intendent)
{
	for(const auto &cNode : node.contentNodes) {
		if(nonstd::holds_alternative<NodeType>(cNode))
			writeNodeDeclaration(nonstd::get<NodeType>(cNode), typeMappings, intendent);
		else if(nonstd::holds_alternative<EntryType>(cNode))
			writeEntryDeclaration(nonstd::get<EntryType>(cNode), typeMappings, intendent);
		else if(nonstd::holds_alternative<ListEntryType>(cNode))
			writeListEntryDeclaration(nonstd::get<ListEntryType>(cNode), typeMappings, intendent);
		else if(nonstd::holds_alternative<NodeContentGroup>(cNode))
			writeNodeElementDeclarations(nonstd::get<NodeContentGroup>(cNode), typeMappings, intendent);
		else
			Q_UNREACHABLE();
	}
}

void CppSettingsGenerator::writeNodeDeclaration(const NodeType &node, const QHash<QString, QString> &typeMappings, int intendent)
{
	_hdr << TABS << "struct { //" << node.key << "\n";
	writeNodeElementDeclarations(node, typeMappings, intendent + 1);
	_hdr << TABS << "} " << node.key << ";\n";
}

void CppSettingsGenerator::writeEntryDeclaration(const EntryType &entry, const QHash<QString, QString> &typeMappings, int intendent)
{
	if(entry.contentNodes.isEmpty())
		_hdr << TABS << "QtMvvm::SettingsEntry<" << typeMappings.value(entry.type, entry.type) << "> " << entry.key << ";\n";
	else {
		const auto &mType = typeMappings.value(entry.type, entry.type);
		_hdr << TABS << "struct : QtMvvm::SettingsEntry<" << mType << "> { //" << entry.key << "\n";
		writeNodeElementDeclarations(entry, typeMappings, intendent + 1);
		_hdr << TABS << "\tinline auto &operator=(const " << mType << " &__value) { SettingsEntry<" << mType << ">::operator=(__value); return *this; }\n";
		_hdr << TABS << "} " << entry.key << ";\n";
	}
}

void CppSettingsGenerator::writeListEntryDeclaration(const SettingsGeneratorBase::ListEntryType &entry, const QHash<QString, QString> &typeMappings, int intendent)
{
	if(entry.contentNodes.isEmpty())
		_hdr << TABS << "QtMvvm::SettingsListEntry<" << typeMappings.value(entry.type, entry.type) << "> " << entry.key << ";\n";
	else {
		const QString mType = typeMappings.value(entry.type, entry.type);
		_hdr << TABS << "struct : QtMvvm::SettingsListEntry<" << mType << "> { //" << entry.key << "\n";
		writeNodeElementDeclarations(entry, typeMappings, intendent + 1);
		_hdr << TABS << "\tinline auto &operator=(const QList<" << mType << "> &__value) { SettingsListEntry<" << mType << ">::operator=(__value); return *this; }\n";
		_hdr << TABS << "\tinline auto &operator+=(const QList<" << mType << "> &__value) { SettingsListEntry<" << mType << ">::operator+=(__value); return *this; }\n";
		_hdr << TABS << "\tinline auto &operator+=(const " << mType << " &__value) { SettingsListEntry<" << mType << ">::operator+=(__value); return *this; }\n";
		_hdr << TABS << "} " << entry.key << ";\n";
	}
}

void CppSettingsGenerator::writeSource(const SettingsType &settings)
{
	_src << "#include \"" << _hdrFile.fileName() << "\"\n";
	_src << "#include <QtCore/QCoreApplication>\n";
	_src << "#include <QtMvvmCore/ServiceRegistry>\n";
	if(!settings.backend)
		_src << "#include <QtMvvmCore/QSettingsAccessor>\n";
	_src << "\n";

	auto backend = settings.backend.value_or(BackendType{QStringLiteral("QtMvvm::QSettingsAccessor"), {}});

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

	_src << settings.name.value() << "::" << settings.name.value() << "(QObject *parent) : \n"
		 << "\t" << settings.name.value() << "{new " << backend.className << "{";
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
	_src << "}, parent}\n"
		 << "{\n"
		 << "\t_accessor->setParent(this);\n"
		 << "}\n\n";

	_src << settings.name.value() << "::" << settings.name.value() << "(QtMvvm::ISettingsAccessor *accessor, QObject *parent) : \n"
		 << "\tQObject{parent},\n"
		 << "\t_accessor{accessor}\n"
		 << "{\n";
	writeNodeElementDefinitions(settings, settings.typeMappings, settings.baseKey);
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

void CppSettingsGenerator::writeNodeElementDefinitions(const SettingsGeneratorBase::NodeContentGroup &node, const QHash<QString, QString> &typeMappings, const optional<QString> &baseKey, const QStringList &keyChain)
{
	for(const auto &cNode : node.contentNodes) {
		if(nonstd::holds_alternative<NodeType>(cNode)) {
			const auto &xNode = nonstd::get<NodeType>(cNode);
			writeNodeElementDefinitions(xNode, typeMappings, baseKey, QStringList{keyChain} << xNode.key);
		} else if(nonstd::holds_alternative<EntryType>(cNode))
			writeEntryDefinition(nonstd::get<EntryType>(cNode), typeMappings, baseKey, keyChain);
		else if(nonstd::holds_alternative<ListEntryType>(cNode))
			writeListEntryDefinition(nonstd::get<ListEntryType>(cNode), typeMappings, baseKey, keyChain);
		else if(nonstd::holds_alternative<NodeContentGroup>(cNode))
			writeNodeElementDefinitions(nonstd::get<NodeContentGroup>(cNode), typeMappings, baseKey, keyChain);
		else
			Q_UNREACHABLE();
	}
}

void CppSettingsGenerator::writeEntryDefinition(const SettingsGeneratorBase::EntryType &entry, const QHash<QString, QString> &typeMappings, const optional<QString> &baseKey, QStringList keyChain, bool skipChildren)
{
	keyChain.append(entry.key);
	_src << "\t" << keyChain.join(QLatin1Char('.'))
		 << ".setup(QStringLiteral(\"" << (baseKey ? QStringList{baseKey.value()} + keyChain : keyChain).join(QLatin1Char('/')) << "\"), _accessor";
	if(entry.code)
		_src << ", QVariant::fromValue<" << typeMappings.value(entry.type, entry.type) << ">(" << entry.code.value().trimmed() << ")";
	else if(entry.defaultValue) {
		_src << ", QVariant{";
		if(entry.tr)
			_src << "QCoreApplication::translate(\"" << entry.trContext.value() << "\", \"" << entry.defaultValue.value() << "\")";
		else
			_src << "QStringLiteral(\"" << entry.defaultValue.value() << "\")";
		_src << "}.value<" << typeMappings.value(entry.type, entry.type) << ">()";
	}
	_src << ");\n";
	if(!skipChildren)
		writeNodeElementDefinitions(entry, typeMappings, baseKey, keyChain);
}

void CppSettingsGenerator::writeListEntryDefinition(const SettingsGeneratorBase::ListEntryType &entry, const QHash<QString, QString> &typeMappings, const optional<QString> &baseKey, QStringList keyChain)
{
	writeEntryDefinition(entry, typeMappings, baseKey, keyChain, entry.init.has_value());
	if(entry.init) {
		keyChain.append(entry.key);
		_src << "\t" << keyChain.join(QLatin1Char('.')) << ".setupInit(" << entry.init.value().trimmed() << ");\n";
		writeNodeElementDefinitions(entry, typeMappings, baseKey, keyChain);
	}
}


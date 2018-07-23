#include "qmlsettingsgenerator.h"
#include <QFileInfo>

QmlSettingsGenerator::QmlSettingsGenerator(const QString &hdrPath, const QString &srcPath) :
	_hdrFile{hdrPath},
	_srcFile{srcPath},
	_hdr{&_hdrFile},
	_src{&_srcFile}
{}

void QmlSettingsGenerator::process(const QString &inPath)
{
	// read settings and adjust defaults
	auto settings = readDocument(inPath);
	if(!settings.name)
		settings.name = QFileInfo{inPath}.baseName();
	settings.name.value().prepend(QStringLiteral("QMLTYPE_"));
	fixTrContext(settings, QFileInfo{inPath}.fileName());

	_name = settings.name.value();
	_prefixName = settings.prefix ? settings.prefix.value() + QLatin1Char(' ') + _name : _name;
	_typeMappings = settings.typeMappings;

	if(!_hdrFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throw FileException{_hdrFile};
	writeHeader(settings, QFileInfo{inPath}.completeBaseName() + QStringLiteral(".h"));
	_hdrFile.close();

	if(!_srcFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throw FileException{_srcFile};
	writeSource(settings);
	_srcFile.close();
}

void QmlSettingsGenerator::writeHeader(const SettingsGeneratorBase::SettingsType &settings, const QString &inHdrPath)
{
	QString incGuard = QFileInfo{_hdrFile.fileName()}
					   .completeBaseName()
					   .replace(QLatin1Char('.'), QLatin1Char('_'))
					   .toUpper() + QStringLiteral("_H");
	_hdr << "#ifndef " << incGuard << '\n'
		 << "#define " << incGuard << "\n\n";

	// write the includes
	const auto &hdrPath = settings.qml.has_value() ? settings.qml.value().header.value_or(inHdrPath) : inHdrPath;
	auto includes = QList<IncludeType> {
		{false, QStringLiteral("QtCore/QObject")},
		{false, QStringLiteral("QtCore/QScopedPointer")},
		{false, QStringLiteral("QtQml/QQmlListProperty")},
		{false, hdrPath}
	} + settings.includes;
	for(const auto &inc : includes) {
		if(inc.local)
			_hdr << "#include \"" << inc.includePath << "\"\n";
		else
			_hdr << "#include <" << inc.includePath << ">\n";
	}
	_hdr << "\n";

	writeListTypeClasses(settings);

	// create all the qmltype classes
	int offset;
	QList<int> childOffsets;
	std::tie(offset, childOffsets) = writeNodeContentClassesDeclarations(settings);

	// create the class
	_hdr << "class " << _prefixName << " : public QObject\n"
		 << "{\n"
		 << "\tQ_OBJECT\n\n"
		 << "\tQ_PROPERTY(QtMvvm::ISettingsAccessor *accessor READ accessor CONSTANT FINAL)\n\n";

	writeNodeClassPropertiesDeclarations(settings, childOffsets);

	_hdr << "public:\n"
		 << "\texplicit " << settings.name.value() << "(QObject *parent = nullptr);\n"
		 << "\tQtMvvm::ISettingsAccessor *accessor() const;\n\n";

	_hdr << "\nprivate:\n"
		 << "\tQtMvvm::ISettingsAccessor *_accessor;\n"
		 << "};\n\n"
		 << "#endif //" << incGuard << '\n';
}

void QmlSettingsGenerator::writeListTypeClasses(const SettingsGeneratorBase::SettingsType &settings)
{
	// write the list wrapper base class
	_hdr << "class " << _prefixName << "_ListType : public QObject\n"
		 << "{\n"
		 << "\tQ_OBJECT\n\n"
		 << "\tQ_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)\n\n"
		 << "public:\n"
		 << "\tclass Accessor {\n"
		 << "\tpublic:\n"
		 << "\t\tinline virtual ~Accessor() = default;\n"
		 << "\t\tvirtual QVariant value() const = 0;\n"
		 << "\t\tvirtual void setValue(const QVariant &value) = 0;\n"
		 << "\t};\n\n"
		 << "\texplicit " << _prefixName << "_ListType(QObject *parent);\n\n"
		 << "\tQVariant value() const { return _data ? _data->value() : _buffer; }\n"
		 << "\tvoid setValue(const QVariant &value) { if(_data) _data->setValue(value); else _buffer = value; }\n"
		 << "\tvoid setAccessor(Accessor *accessor) { _data.reset(accessor); if(_buffer.isValid()) _data->setValue(_buffer); _buffer.clear(); }\n\n"
		 << "Q_SIGNALS:\n"
		 << "\tvoid valueChanged(const QVariant &value);\n\n"
		 << "private:\n"
		 << "\tQScopedPointer<Accessor> _data;\n"
		 << "\tQVariant _buffer;\n"
		 << "};\n\n";

	// write the generic variant
	_hdr << "template <typename T>\n"
		 << "class " << _prefixName << "_ListType_Accessor : public " << _prefixName << "_ListType::Accessor\n"
		 << "{\n"
		 << "public:\n"
		 << "\tstruct ListData {\n"
		 << "\t\tQtMvvm::SettingsEntry<QList<T>> &entry;\n"
		 << "\t\tQList<" << _prefixName << "_ListType> elements;\n"
		 << "\t};\n\n"
		 << "\t" << _prefixName << "_ListType_Accessor(QtMvvm::SettingsEntry<QList<T>> &entry, int index) : \n"
		 << "\t\t_entry{entry},\n"
		 << "\t\t_index{index}\n"
		 << "\t{}\n"
		 << "\tQVariant value() const final { return QVariant::fromValue(_entry.getAt(_index)); }\n"
		 << "\tvoid setValue(const QVariant &value) final { _entry.setAt(_index, value.template value<T>()); }\n\n"
		 << "private:\n"
		 << "\tfriend class " << _prefixName << ";\n"
		 << "\tQtMvvm::SettingsEntry<QList<T>> &_entry;\n"
		 << "\tconst int _index;\n\n"
		 << "\tstatic void append(QQmlListProperty<" << _prefixName << "_ListType>* list, " << _prefixName << "_ListType* element) {\n"
		 << "\t\tconst auto data = reinterpret_cast<ListData*>(list->data);\n"
		 << "\t\tconst auto maxIndex = data->entry.size();\n"
		 << "\t\tfor(auto index = data->elements.size(); index <= maxIndex; index++) {\n"
		 << "\t\t\t" << _prefixName << "_ListType *elem;\n"
		 << "\t\t\tif(index == maxIndex) {\n"
		 << "\t\t\t\telem = element;\n"
		 << "\t\t\t\telem->setParent(list->object);\n"
		 << "\t\t\t} else\n"
		 << "\t\t\t\telem = new " << _prefixName << "_ListType{list->object};\n"
		 << "\t\t\tdata->elements.append(elem);\n"
		 << "\t\t\telem->setAccessor(new " << _prefixName << "_ListType_Accessor<T>{data->entry, index});\n"
		 << "\t\t\tdata->entry.addChangeCallback(elem, [index, elem](int i, const T &d) {\n"
		 << "\t\t\t\tif(i == index)\n"
		 << "\t\t\t\t\temit elem->valueChanged(QVariant::fromValue(d));\n"
		 << "\t\t\t});\n"
		 << "\t\t}\n"
		 << "\t}\n\n"
		 << "\tstatic " << _prefixName << "_ListType *at(QQmlListProperty<" << _prefixName << "_ListType>* list, int index) {\n"
		 << "\t\tconst auto data = reinterpret_cast<ListData*>(list->data);\n"
		 << "\t\treturn data->elements.size() > index ? data->elements.value(index) : nullptr;\n"
		 << "\t}\n\n"
		 << "\tstatic int count(QQmlListProperty<" << _prefixName << "_ListType>* list) {\n"
		 << "\t\treturn reinterpret_cast<ListData*>(list->data)->elements.size();\n"
		 << "\t}\n\n"
		 << "\tstatic void clear(QQmlListProperty<" << _prefixName << "_ListType>* list) {\n"
		 << "\t\tconst auto data = reinterpret_cast<ListData*>(list->data);\n"
		 << "\t\tfor(auto elem : qAsConst(data->elements))\n"
		 << "\t\t\telem->deleteLater();\n"
		 << "\t\tdata->elements.clear();\n"
		 << "\t\tdata->entry.reset(false);\n"
		 << "\t}\n"
		 << "};\n\n";
}

std::tuple<int, QList<int>> QmlSettingsGenerator::writeNodeContentClassesDeclarations(const SettingsGeneratorBase::NodeContentGroup &node, int offset)
{
	QList<int> offsetList;
	for(const auto &cNode : node.contentNodes) {
		if(nonstd::holds_alternative<NodeType>(cNode)) {
			offset = writeNodeClassDeclaration(nonstd::get<NodeType>(cNode), offset);
			offsetList.append(offset - 1);
		} else if(nonstd::holds_alternative<EntryType>(cNode)) {
			if(!nonstd::get<EntryType>(cNode).contentNodes.isEmpty()) {
				offset = writeNodeClassDeclaration(nonstd::get<EntryType>(cNode), offset);
				offsetList.append(offset - 1);
			} else
				offsetList.append(-1);
		} else if(nonstd::holds_alternative<ListEntryType>(cNode)) {
			if(!nonstd::get<ListEntryType>(cNode).contentNodes.isEmpty()) {
				offset = writeNodeClassDeclaration(nonstd::get<ListEntryType>(cNode), offset);
				offsetList.append(offset - 1);
			} else
				offsetList.append(-1);
		} else if(nonstd::holds_alternative<NodeContentGroup>(cNode)) {
			QList<int> subList;
			std::tie(offset, subList) = writeNodeContentClassesDeclarations(nonstd::get<NodeContentGroup>(cNode), offset);
			offsetList.append(subList);
		} else
			Q_UNREACHABLE();
	}
	return std::make_tuple(offset, offsetList);
}

void QmlSettingsGenerator::writeNodeClassPropertiesDeclarations(const SettingsGeneratorBase::NodeContentGroup &node, QList<int> &childOffsets)
{
	for(const auto &cNode : node.contentNodes) {
		if(nonstd::holds_alternative<NodeType>(cNode))
			writeNodePropertyDeclaration(nonstd::get<NodeType>(cNode), childOffsets.takeFirst());
		else if(nonstd::holds_alternative<EntryType>(cNode))
			writeEntryPropertyDeclaration(nonstd::get<EntryType>(cNode), childOffsets.takeFirst());
		else if(nonstd::holds_alternative<ListEntryType>(cNode))
			writeEntryPropertyDeclaration(nonstd::get<ListEntryType>(cNode), childOffsets.takeFirst());
		else if(nonstd::holds_alternative<NodeContentGroup>(cNode))
			writeNodeClassPropertiesDeclarations(nonstd::get<NodeContentGroup>(cNode), childOffsets);
		else
			Q_UNREACHABLE();
	}
}

void QmlSettingsGenerator::writeNodePropertyDeclaration(const SettingsGeneratorBase::NodeType &entry, int classIndex, const QString &overwriteName)
{
	const auto &mName = overwriteName.isNull() ? entry.key : overwriteName;
	_hdr << "\tQ_PROPERTY(" << _name << "_" << classIndex << "* " << mName
		 << " MEMBER _" << mName << " CONSTANT)\n";

	_hdr << "\t" << _name << "_" << classIndex << "* _" << mName << ";\n\n";
}

void QmlSettingsGenerator::writeEntryPropertyDeclaration(const SettingsGeneratorBase::EntryType &entry, int classIndex)
{
	const auto &mType = _typeMappings.value(entry.type, entry.type);
	if(mType != QStringLiteral("void")) {
		_hdr << "\tQ_PROPERTY(" << mType << " " << entry.key
			 << " READ get_" << entry.key
			 << " WRITE set_" << entry.key
			 << " NOTIFY notify_" << entry.key << ")\n";

		_hdr << "\t" << mType << " get_" << entry.key << "() const;\n"
			 << "\tvoid set_" << entry.key << "(const " << mType << "& value);\n"
			 << "Q_SIGNALS:\n"
			 << "\tvoid notify_" << entry.key << "();\n"
			 << "private:\n\n";
	}

	if(!entry.contentNodes.isEmpty())
		writeNodePropertyDeclaration(entry, classIndex, entry.qmlGroupKey.value_or(entry.key + QStringLiteral("Group")));
}

int QmlSettingsGenerator::writeNodeClassDeclaration(const SettingsGeneratorBase::NodeType &node, int offset)
{
	QList<int> childOffsets;
	std::tie(offset, childOffsets) = writeNodeContentClassesDeclarations(node, offset);

	_hdr << "class " << _prefixName << "_" << offset << " : public QObject // " << node.key << "\n"
		 << "{\n"
		 << "\tQ_OBJECT\n\n";

	_hdr << "public:\n"
		 << "\t" << _name << "_" << offset << "(QtMvvm::ISettingsAccessor *accessor, const QString &parentKey, QObject *parent);\n\n"
		 << "private:\n"
		 << "\tQtMvvm::ISettingsAccessor *_accessor;\n"
		 << "\tQString _nodeKey;\n\n";

	writeNodeClassPropertiesDeclarations(node, childOffsets);

	_hdr << "};\n\n";

	return ++offset;
}

void QmlSettingsGenerator::writeSource(const SettingsGeneratorBase::SettingsType &settings)
{
	_src << "#include \"" << _hdrFile.fileName() << "\"\n\n";

	_src << "QtMvvm::ISettingsAccessor *" << settings.name.value() << "::accessor() const\n"
		 << "{\n"
		 << "\treturn _accessor;\n"
		 << "}\n\n";
}

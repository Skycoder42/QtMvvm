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
	_cppName = settings.name.value();
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

	writeListTypeBaseClass(settings);

	// create all the qmltype classes
	auto keyList = settings.prefix ? QStringList{settings.prefix.value()} : QStringList{};
	int offset;
	QList<int> childOffsets;
	std::tie(offset, childOffsets) = writeNodeContentClassesDeclarations(settings, keyList);

	// create the class
	_hdr << "class " << _prefixName << " : public QObject\n"
		 << "{\n"
		 << "\tQ_OBJECT\n\n"
		 << "\tQ_PROPERTY(QtMvvm::ISettingsAccessor *accessor READ accessor CONSTANT FINAL)\n\n";

	QList<QPair<QString, int>> childConstructs;
	writeNodeClassPropertiesDeclarations(settings, keyList, childOffsets, childConstructs);

	_hdr << "public:\n"
		 << "\texplicit " << settings.name.value() << "(QObject *parent = nullptr);\n"
		 << "\tQtMvvm::ISettingsAccessor *accessor() const;\n\n";

	_hdr << "\nprivate:\n"
		 << "\tQtMvvm::ISettingsAccessor *_accessor;\n"
		 << "};\n\n"
		 << "#endif //" << incGuard << '\n';
}

void QmlSettingsGenerator::writeListTypeBaseClass(const SettingsGeneratorBase::SettingsType &settings)
{
	// write the generic variant
	_hdr << "template <typename T>\n"
		 << "class " << _prefixName << "_ListType : public QObject\n"
		 << "{\n"
		 << "public:\n"
		 << "\ttemplate <typename TList>\n"
		 << "\tstruct ListData {\n"
		 << "\t\tstatic_assert(std::is_base_of<" << _prefixName << "_ListType<T>, TList>::value, \"TList must extend " << _prefixName << "_ListType<T>\");\n"
		 << "\t\tQtMvvm::SettingsEntry<QList<T>> &entry;\n"
		 << "\t\tQList<TList*> elements;\n"
		 << "\t};\n\n"
		 << "\t" << _prefixName << "_ListType(QtMvvm::SettingsEntry<QList<T>> &entry, QObject *parent) : \n"
		 << "\t\tQObject{parent},\n"
		 << "\t\t_entry{entry}\n"
		 << "\t{}\n"
		 << "\tT value() const { return _index >= 0 ? _entry.getAt(_index) : _buffer; }\n"
		 << "\tvoid setValue(const T &value) { if(_index >= 0) _entry.setAt(_index, value); else { _index = -2; _buffer = value; } }\n\n"
		 << "private:\n"
		 << "\tfriend class " << _prefixName << ";\n"
		 << "\tQtMvvm::SettingsEntry<QList<T>> &_entry;\n"
		 << "\tint _index = -1;\n"
		 << "\tT _buffer{};\n\n"
		 << "\ttemplate <typename TList>\n"
		 << "\tstatic void append(QQmlListProperty<TList> *list, TList *element) {\n"
		 << "\t\tconst auto data = reinterpret_cast<ListData<TList>*>(list->data);\n"
		 << "\t\tconst auto maxIndex = data->entry.size();\n"
		 << "\t\tfor(auto index = data->elements.size(); index <= maxIndex; index++) {\n"
		 << "\t\t\tTList *elem;\n"
		 << "\t\t\tif(index == maxIndex) {\n"
		 << "\t\t\t\telem = element;\n"
		 << "\t\t\t\telem->setParent(list->object);\n"
		 << "\t\t\t} else\n"
		 << "\t\t\t\telem = new TList{list->object};\n"
		 << "\t\t\tconst auto copyDefault = elem->_index == -2;\n"
		 << "\t\t\telem->_index = index;\n"
		 << "\t\t\tdata->elements.append(elem);\n"
		 << "\t\t\tif(copyDefault)\n"
		 << "\t\t\t\telem->setValue(T{std::move(elem->_buffer)});\n"
		 << "\t\t\tdata->entry.addChangeCallback(elem, [index, elem](int i, const T &d) {\n"
		 << "\t\t\t\tif(i == index)\n"
		 << "\t\t\t\t\temit elem->valueChanged(d);\n"
		 << "\t\t\t});\n"
		 << "\t\t}\n"
		 << "\t}\n\n"
		 << "\ttemplate <typename TList>\n"
		 << "\tstatic TList *at(QQmlListProperty<TList> *list, int index) {\n"
		 << "\t\tconst auto data = reinterpret_cast<ListData<TList>*>(list->data);\n"
		 << "\t\treturn data->elements.size() > index ? data->elements.value(index) : nullptr;\n"
		 << "\t}\n\n"
		 << "\ttemplate <typename TList>\n"
		 << "\tstatic int count(QQmlListProperty<TList> *list) {\n"
		 << "\t\treturn reinterpret_cast<ListData<TList>*>(list->data)->elements.size();\n"
		 << "\t}\n\n"
		 << "\ttemplate <typename TList>\n"
		 << "\tstatic void clear(QQmlListProperty<TList> *list) {\n"
		 << "\t\tconst auto data = reinterpret_cast<ListData<TList>*>(list->data);\n"
		 << "\t\tfor(auto elem : qAsConst(data->elements))\n"
		 << "\t\t\telem->deleteLater();\n"
		 << "\t\tdata->elements.clear();\n"
		 << "\t\tdata->entry.reset(false);\n"
		 << "\t}\n"
		 << "};\n\n";
}

std::tuple<int, QList<int>> QmlSettingsGenerator::writeNodeContentClassesDeclarations(const SettingsGeneratorBase::NodeContentGroup &node, const QStringList &keyList, int offset)
{
	QList<int> offsetList;
	for(const auto &cNode : node.contentNodes) {
		if(nonstd::holds_alternative<NodeType>(cNode)) {
			offset = writeNodeClassDeclaration(nonstd::get<NodeType>(cNode), keyList, offset);
			offsetList.append(offset - 1);
		} else if(nonstd::holds_alternative<EntryType>(cNode)) {
			if(!nonstd::get<EntryType>(cNode).contentNodes.isEmpty()) {
				offset = writeNodeClassDeclaration(nonstd::get<EntryType>(cNode), keyList, offset);
				offsetList.append(offset - 1);
			} else
				offsetList.append(-1);
		} else if(nonstd::holds_alternative<ListEntryType>(cNode)) {
			offset = writeListEntryListClass(nonstd::get<ListEntryType>(cNode), keyList, offset);
			offsetList.append(offset - 1); //double offset!!!
			if(!nonstd::get<ListEntryType>(cNode).contentNodes.isEmpty()) {
				offset = writeNodeClassDeclaration(nonstd::get<ListEntryType>(cNode), keyList, offset);
				offsetList.append(offset - 1);
			} else
				offsetList.append(-1);
		} else if(nonstd::holds_alternative<NodeContentGroup>(cNode)) {
			QList<int> subList;
			std::tie(offset, subList) = writeNodeContentClassesDeclarations(nonstd::get<NodeContentGroup>(cNode), keyList, offset);
			offsetList.append(subList);
		} else
			Q_UNREACHABLE();
	}
	return std::make_tuple(offset, offsetList);
}

int QmlSettingsGenerator::writeNodeClassDeclaration(const SettingsGeneratorBase::NodeType &node, const QStringList &keyList, int offset)
{
	auto subList = QStringList{keyList} << node.key;
	QList<int> childOffsets;
	std::tie(offset, childOffsets) = writeNodeContentClassesDeclarations(node, subList, offset);

	_hdr << "class " << _prefixName << "_" << offset << " : public QObject // " << node.key << "\n"
		 << "{\n"
		 << "\tQ_OBJECT\n\n";

	QList<QPair<QString, int>> childConstructs;
	writeNodeClassPropertiesDeclarations(node, subList, childOffsets, childConstructs);

	_hdr << "\t" << _cppName << " *_settings;\n\n"
		 << "public:\n"
		 << "\t" << _name << "_" << offset << "(" << _cppName << " *settings, QObject *parent) : \n"
		 << "\t\tQObject{parent},\n";
	for(const auto &info : childConstructs)
		_hdr << "\t\t_" << info.first << "{new " << _name << "_" << info.second << "{settings, this}},\n";
	_hdr << "\t\t_settings{settings}\n"
		 << "\t{}\n"
		 << "};\n\n";

	return ++offset;
}

int QmlSettingsGenerator::writeListEntryListClass(const SettingsGeneratorBase::ListEntryType &entry, QStringList keyList, int offset)
{
	const auto &mType = _typeMappings.value(entry.type, entry.type);
	_hdr << "class " << _prefixName << "_" << offset << " : public " << _prefixName << "_ListType<" << mType << "> // " << entry.key << "\n"
		 << "{\n"
		 << "\tQ_OBJECT\n\n"
		 << "\tQ_PROPERTY(" << mType << " value READ value WRITE setValue NOTIFY valueChanged)\n\n"
		 << "public:\n"
		 << "\texplicit " << _name << "_" << offset << "(QObject *parent = nullptr);\n\n"
		 << "Q_SIGNALS:\n"
		 << "\tvoid valueChanged(const " << mType << " &value);\n"
		 << "};\n\n";

	return ++offset;
}

void QmlSettingsGenerator::writeNodeClassPropertiesDeclarations(const SettingsGeneratorBase::NodeContentGroup &node, const QStringList &keyList, QList<int> &childOffsets, QList<QPair<QString, int>> &childConstructs)
{
	for(const auto &cNode : node.contentNodes) {
		if(nonstd::holds_alternative<NodeType>(cNode))
			writeNodePropertyDeclaration(nonstd::get<NodeType>(cNode), childOffsets.takeFirst(), childConstructs);
		else if(nonstd::holds_alternative<EntryType>(cNode))
			writeEntryPropertyDeclaration(nonstd::get<EntryType>(cNode), keyList, childOffsets.takeFirst(), childConstructs);
		else if(nonstd::holds_alternative<ListEntryType>(cNode)) {
			childOffsets.takeFirst(); //TODO use...
			writeEntryPropertyDeclaration(nonstd::get<ListEntryType>(cNode), keyList, childOffsets.takeFirst(), childConstructs);
		} else if(nonstd::holds_alternative<NodeContentGroup>(cNode))
			writeNodeClassPropertiesDeclarations(nonstd::get<NodeContentGroup>(cNode), keyList, childOffsets, childConstructs);
		else
			Q_UNREACHABLE();
	}
}

void QmlSettingsGenerator::writeNodePropertyDeclaration(const SettingsGeneratorBase::NodeType &entry, int classIndex, QList<QPair<QString, int>> &childConstructs, const QString &overwriteName)
{
	const auto &mName = overwriteName.isNull() ? entry.key : overwriteName;
	_hdr << "\tQ_PROPERTY(" << _name << "_" << classIndex << "* " << mName
		 << " MEMBER _" << mName << " CONSTANT)\n";
	_hdr << "\t" << _name << "_" << classIndex << "* _" << mName << ";\n\n";
	childConstructs.append({mName, classIndex});
}

void QmlSettingsGenerator::writeEntryPropertyDeclaration(const SettingsGeneratorBase::EntryType &entry, QStringList keyList, int classIndex, QList<QPair<QString, int>> &childConstructs)
{
	keyList.append(entry.key);
	const auto &mType = _typeMappings.value(entry.type, entry.type);
	if(mType != QStringLiteral("void")) {
		_hdr << "\tQ_PROPERTY(" << mType << " " << entry.key
			 << " READ get_" << entry.key
			 << " WRITE set_" << entry.key
			 << " NOTIFY " << entry.key << "_changed)\n";

		_hdr << "\t" << mType << " get_" << entry.key << "() const { return _settings->" << keyList.join(QLatin1Char('.')) << ".get(); }\n"
			 << "\tvoid set_" << entry.key << "(const " << mType << "& value) { _settings->" << keyList.join(QLatin1Char('.')) << ".set(value); }\n"
			 << "Q_SIGNALS:\n"
			 << "\tvoid " << entry.key << "_changed();\n"
			 << "private:\n\n";
	}

	if(!entry.contentNodes.isEmpty())
		writeNodePropertyDeclaration(entry, classIndex, childConstructs, entry.qmlGroupKey.value_or(entry.key + QStringLiteral("Group")));
}

void QmlSettingsGenerator::writeSource(const SettingsGeneratorBase::SettingsType &settings)
{
	_src << "#include \"" << _hdrFile.fileName() << "\"\n\n";

	_src << "QtMvvm::ISettingsAccessor *" << settings.name.value() << "::accessor() const\n"
		 << "{\n"
		 << "\treturn _accessor;\n"
		 << "}\n\n";
}

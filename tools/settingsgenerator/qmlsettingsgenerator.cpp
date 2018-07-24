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

void QmlSettingsGenerator::writeHeader(const SettingsType &settings, const QString &inHdrPath)
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

	writeListTypeBaseClass();

	// create all the qmltype classes
	auto keyList = settings.baseKey ? QStringList{settings.baseKey.value()} : QStringList{};
	int offset;
	QList<int> childOffsets;
	std::tie(offset, childOffsets) = writeNodeContentClasses(settings, keyList);

	// create the class
	_hdr << "class " << _name << " : public QObject\n"
		 << "{\n"
		 << "\tQ_OBJECT\n\n"
		 << "\tQ_PROPERTY(QtMvvm::ISettingsAccessor *accessor READ accessor CONSTANT FINAL)\n\n";

	QList<QPair<QString, int>> childConstructs;
	QList<int> listEntries;
	writeProperties(settings, keyList, childOffsets, listEntries, childConstructs);

	_hdr << "\t" << _cppName << " *_settings;\n\n"
		 << "public:\n"
		 << "\texplicit " << _name << "(" << _cppName << " *settings, QObject *parent = nullptr) : \n"
		 << "\t\tQObject{parent},\n";
	writeMemberInits(keyList, childConstructs);
	_hdr << "\t\t_settings{settings}\n"
		 << "\t{\n";
	writeEntryPropertySignalConnects(settings, keyList, -1, listEntries);
	_hdr << "\t}\n\n"
		 << "\texplicit " << _name << "(QObject *parent = nullptr) :\n"
		 << "\t\t" << _name << "{" << _cppName << "::instance(), parent}\n"
		 << "\t{}\n\n"
		 << "\tQtMvvm::ISettingsAccessor *accessor() const { return _settings->accessor(); }\n"
		 << "\t" << _cppName << " *settings() const { return _settings; }\n\n"
		 << "\t" << (settings.prefix ? settings.prefix.value() + QLatin1Char(' ') : QString{}) << "static void registerQmlTypes(const char *uri, int major, int minor);\n";
	if(settings.qml)
		_hdr << "\t" << (settings.prefix ? settings.prefix.value() + QLatin1Char(' ') : QString{}) << "static void registerQmlTypes();\n";
	_hdr << "};\n\n"
		 << "#endif //" << incGuard << '\n';
}

void QmlSettingsGenerator::writeListTypeBaseClass()
{
	// write the generic variant
	_hdr << "template <typename T>\n"
		 << "class " << _name << "_ListType : public QObject\n"
		 << "{\n"
		 << "public:\n"
		 << "\ttemplate <typename TList>\n"
		 << "\tstruct ListData {\n"
		 << "\t\tstatic_assert(std::is_base_of<" << _name << "_ListType<T>, TList>::value, \"TList must extend " << _name << "_ListType<T>\");\n"
		 << "\t\tQtMvvm::SettingsListEntry<T> &entry;\n"
		 << "\t\tQList<TList*> elements;\n"
		 << "\t};\n\n"
		 << "\t" << _name << "_ListType(QObject *parent) : \n"
		 << "\t\tQObject{parent}\n"
		 << "\t{}\n"
		 << "\tT value() const { return _entry ? _entry->getAt(_index) : _buffer; }\n"
		 << "\tvoid setValue(const T &value) { if(_entry) _entry->setAt(_index, value); else { _index = -2; _buffer = value; } }\n\n"

		 << "\ttemplate <typename TList>\n"
		 << "\tstatic void adjust(ListData<TList> *data, QObject *parent, int size) {\n"
		 << "\t\twhile(data->elements.size() > size)\n"
		 << "\t\t\tdata->elements.takeLast()->deleteLater();\n"
		 << "\t\tfor(auto index = data->elements.size(); index < size; index++) {\n"
		 << "\t\t\tauto elem = new TList{parent};\n"
		 << "\t\t\telem->_index = index;\n"
		 << "\t\t\telem->_entry = &data->entry;\n"
		 << "\t\t\tdata->elements.append(elem);\n"
		 << "\t\t\tdata->entry.addChangeCallback(elem, [index, elem](int i, const T &d) {\n"
		 << "\t\t\t\tif(i == index)\n"
		 << "\t\t\t\t\temit elem->valueChanged(d);\n"
		 << "\t\t\t});\n"
		 << "\t\t}\n"
		 << "\t}\n\n"

		 << "\ttemplate <typename TList>\n"
		 << "\tstatic void append(QQmlListProperty<TList> *list, TList *element) {\n"
		 << "\t\tconst auto data = reinterpret_cast<ListData<TList>*>(list->data);\n"
		 << "\t\tconst auto maxIndex = data->entry.size();\n"
		 << "\t\tadjust<TList>(data, list->object, maxIndex);\n"
		 << "\t\telement->setParent(list->object);\n"
		 << "\t\tconst auto copyDefault = element->_index == -2;\n"
		 << "\t\telement->_index = maxIndex;\n"
		 << "\t\telement->_entry = &data->entry;\n"
		 << "\t\tdata->elements.append(element);\n"
		 << "\t\tif(copyDefault)\n"
		 << "\t\t\telement->setValue(T{std::move(element->_buffer)});\n"
		 << "\t\tdata->entry.addChangeCallback(element, [maxIndex, element](int i, const T &d) {\n"
		 << "\t\t\tif(i == maxIndex)\n"
		 << "\t\t\t\temit element->valueChanged(d);\n"
		 << "\t\t});\n"
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
		 << "\t}\n\n"

		 << "private:\n"
		 << "\tQtMvvm::SettingsListEntry<T> *_entry = nullptr;\n"
		 << "\tint _index = -1;\n"
		 << "\tT _buffer{};\n"
		 << "};\n\n";
}

std::tuple<int, QList<int>> QmlSettingsGenerator::writeNodeContentClasses(const NodeContentGroup &node, const QStringList &keyList, int offset)
{
	QList<int> offsetList;
	for(const auto &cNode : node.contentNodes) {
		if(nonstd::holds_alternative<NodeType>(cNode)) {
			offset = writeNodeClass(nonstd::get<NodeType>(cNode), keyList, offset);
			offsetList.append(offset - 1);
		} else if(nonstd::holds_alternative<EntryType>(cNode)) {
			if(!nonstd::get<EntryType>(cNode).contentNodes.isEmpty()) {
				offset = writeNodeClass(nonstd::get<EntryType>(cNode), keyList, offset);
				offsetList.append(offset - 1);
			} else
				offsetList.append(-1);
		} else if(nonstd::holds_alternative<ListEntryType>(cNode)) {
			offset = writeListEntryElementClass(nonstd::get<ListEntryType>(cNode), keyList, offset);
			offsetList.append(offset - 1); //double offset!!!
			if(!nonstd::get<ListEntryType>(cNode).contentNodes.isEmpty()) {
				offset = writeNodeClass(nonstd::get<ListEntryType>(cNode), keyList, offset);
				offsetList.append(offset - 1);
			} else
				offsetList.append(-1);
		} else if(nonstd::holds_alternative<NodeContentGroup>(cNode)) {
			QList<int> subList;
			std::tie(offset, subList) = writeNodeContentClasses(nonstd::get<NodeContentGroup>(cNode), keyList, offset);
			offsetList.append(subList);
		} else
			Q_UNREACHABLE();
	}
	return std::make_tuple(offset, offsetList);
}

int QmlSettingsGenerator::writeNodeClass(const NodeType &node, QStringList keyList, int offset)
{
	keyList.append(node.key);
	QList<int> childOffsets;
	std::tie(offset, childOffsets) = writeNodeContentClasses(node, keyList, offset);

	_hdr << "class " << _name << "_" << offset << " : public QObject // " << keyList.join(QLatin1Char('/')) << "\n"
		 << "{\n"
		 << "\tQ_OBJECT\n\n";

	QList<QPair<QString, int>> childConstructs;
	QList<int> listEntries;
	writeProperties(node, keyList, childOffsets, listEntries, childConstructs);

	_hdr << "\t" << _cppName << " *_settings;\n\n"
		 << "public:\n"
		 << "\t" << _name << "_" << offset << "(" << _cppName << " *settings, QObject *parent) : \n"
		 << "\t\tQObject{parent},\n";
	writeMemberInits(keyList, childConstructs);
	_hdr << "\t\t_settings{settings}\n"
		 << "\t{\n";
	writeEntryPropertySignalConnects(node, keyList, offset, listEntries);
	_hdr << "\t}\n"
		 << "};\n\n";

	return ++offset;
}

int QmlSettingsGenerator::writeListEntryElementClass(const ListEntryType &entry, QStringList keyList, int offset)
{
	keyList.append(entry.key);
	const auto &mType = _typeMappings.value(entry.type, entry.type);
	_hdr << "class " << _name << "_" << offset << " : public " << _name << "_ListType<" << mType << "> // " << keyList.join(QLatin1Char('/')) << " (list-element)\n"
		 << "{\n"
		 << "\tQ_OBJECT\n\n"
		 << "\tQ_PROPERTY(" << mType << " value READ value WRITE setValue NOTIFY valueChanged)\n\n"
		 << "public:\n"
		 << "\texplicit " << _name << "_" << offset << "(QObject *parent = nullptr) : \n"
		 << "\t\t" << _name << "_ListType{parent}\n"
		 << "\t{}\n\n"
		 << "Q_SIGNALS:\n"
		 << "\tvoid valueChanged(const " << mType << " &value);\n"
		 << "};\n\n";

	return ++offset;
}

void QmlSettingsGenerator::writeProperties(const NodeContentGroup &node, const QStringList &keyList, QList<int> &childOffsets, QList<int> &listEntries, QList<QPair<QString, int>> &childConstructs)
{
	for(const auto &cNode : node.contentNodes) {
		if(nonstd::holds_alternative<NodeType>(cNode))
			writeNodeProperty(nonstd::get<NodeType>(cNode), childOffsets.takeFirst(), childConstructs);
		else if(nonstd::holds_alternative<EntryType>(cNode))
			writeEntryProperty(nonstd::get<EntryType>(cNode), keyList, childOffsets.takeFirst(), childConstructs);
		else if(nonstd::holds_alternative<ListEntryType>(cNode)) {
			auto lIndex = childOffsets.takeFirst(); //done seperately because of undefine param call order
			writeListEntryProperty(nonstd::get<ListEntryType>(cNode), keyList, lIndex, childOffsets.takeFirst(), childConstructs);
			listEntries.append(lIndex);
		} else if(nonstd::holds_alternative<NodeContentGroup>(cNode))
			writeProperties(nonstd::get<NodeContentGroup>(cNode), keyList, childOffsets, listEntries, childConstructs);
		else
			Q_UNREACHABLE();
	}
}

void QmlSettingsGenerator::writeNodeProperty(const NodeType &entry, int classIndex, QList<QPair<QString, int>> &childConstructs, const QString &overwriteName)
{
	const auto &mName = overwriteName.isNull() ? entry.key : overwriteName;
	_hdr << "\tQ_PROPERTY(" << _name << "_" << classIndex << "* " << mName
		 << " MEMBER _" << mName << " CONSTANT)\n"
		 << "\t" << _name << "_" << classIndex << "* _" << mName << ";\n\n";
	childConstructs.append({mName, classIndex});
}

void QmlSettingsGenerator::writeEntryProperty(const EntryType &entry, QStringList keyList, int classIndex, QList<QPair<QString, int>> &childConstructs)
{
	keyList.append(entry.key);
	const auto &mType = _typeMappings.value(entry.type, entry.type);
	if(mType != QStringLiteral("void")) {
		_hdr << "\tQ_PROPERTY(" << mType << " " << entry.key
			 << " READ get_" << entry.key
			 << " WRITE set_" << entry.key
			 << " NOTIFY " << entry.key << "_changed)\n";

		_hdr << "\t" << mType << " get_" << entry.key << "() const { return _settings->" << keyList.join(QLatin1Char('.')) << ".get(); }\n"
			 << "\tvoid set_" << entry.key << "(const " << mType << " &value) { _settings->" << keyList.join(QLatin1Char('.')) << ".set(value); }\n"
			 << "Q_SIGNALS:\n"
			 << "\tvoid " << entry.key << "_changed(const " << mType << " &value);\n"
			 << "private:\n\n";
	}

	if(!entry.contentNodes.isEmpty())
		writeNodeProperty(entry, classIndex, childConstructs, entry.qmlGroupKey.value_or(entry.key + QStringLiteral("Group")));
}

void QmlSettingsGenerator::writeListEntryProperty(const ListEntryType &entry, QStringList keyList, int listIndex, int classIndex, QList<QPair<QString, int>> &childConstructs)
{
	keyList.append(entry.key);
	_hdr << "\tQ_PROPERTY(QQmlListProperty<" << _name << "_" << listIndex << "> " << entry.key
		 << " READ get_" << entry.key << " CONSTANT)\n"
		 << "\t" << _name << "_" << listIndex << "::ListData<" << _name << "_" << listIndex << "> _" << entry.key << ";\n"
		 << "\tQQmlListProperty<" << _name << "_" << listIndex << "> get_" << entry.key << "() {\n"
		 << "\t\treturn {\n"
		 << "\t\t\tthis, &_" << entry.key << ",\n"
		 << "\t\t\t&" << _name << "_" << listIndex << "::append<" << _name << "_" << listIndex << ">,\n"
		 << "\t\t\t&" << _name << "_" << listIndex << "::count<" << _name << "_" << listIndex << ">,\n"
		 << "\t\t\t&" << _name << "_" << listIndex << "::at<" << _name << "_" << listIndex << ">,\n"
		 << "\t\t\t&" << _name << "_" << listIndex << "::clear<" << _name << "_" << listIndex << ">\n"
		 << "\t\t};\n"
		 << "\t}\n\n";

	childConstructs.append({entry.key, -1});

	if(!entry.contentNodes.isEmpty())
		writeNodeProperty(entry, classIndex, childConstructs, entry.qmlGroupKey.value_or(entry.key + QStringLiteral("Group")));
}

void QmlSettingsGenerator::writeMemberInits(const QStringList &keyList, const QList<QPair<QString, int>> &childConstructs)
{
	for(const auto &info : childConstructs) {
		if(info.second < 0)
			_hdr << "\t\t_" << info.first << "{settings->" << (QStringList{keyList} << info.first).join(QLatin1Char('.')) << ", {}},\n";
		else
			_hdr << "\t\t_" << info.first << "{new " << _name << "_" << info.second << "{settings, this}},\n";
	}
}

void QmlSettingsGenerator::writeEntryPropertySignalConnects(const NodeContentGroup &node, const QStringList &keyList, int classIndex, QList<int> &listEntries)
{
	for(const auto &cNode : node.contentNodes) {
		if(nonstd::holds_alternative<NodeType>(cNode))
			continue; //nothing to be set
		else if(nonstd::holds_alternative<EntryType>(cNode))
			writeEntryPropertySignalConnect(nonstd::get<EntryType>(cNode), keyList, classIndex);
		else if(nonstd::holds_alternative<ListEntryType>(cNode))
			writeListEntryPropertySignalConnect(nonstd::get<ListEntryType>(cNode), keyList, listEntries);
		else if(nonstd::holds_alternative<NodeContentGroup>(cNode))
			writeEntryPropertySignalConnects(nonstd::get<NodeContentGroup>(cNode), keyList, classIndex, listEntries);
		else
			Q_UNREACHABLE();
	}
}

void QmlSettingsGenerator::writeEntryPropertySignalConnect(const EntryType &entry, QStringList keyList, int classIndex)
{
	if(_typeMappings.value(entry.type, entry.type) == QStringLiteral("void"))
		return;
	keyList.append(entry.key);
	_hdr << "\t\t_settings->" << keyList.join(QLatin1Char('.'))
		 << ".addChangeCallback(this, std::bind(&" << _name;
	if(classIndex >= 0)
		_hdr << "_" << classIndex;
	_hdr << "::" << entry.key << "_changed, this, std::placeholders::_1));\n";
}

void QmlSettingsGenerator::writeListEntryPropertySignalConnect(const SettingsGeneratorBase::ListEntryType &entry, QStringList keyList, QList<int> &listEntries)
{
	auto classIndex = listEntries.takeFirst();
	keyList.append(entry.key);
	_hdr << "\t\t_settings->" << keyList.join(QLatin1Char('.'))
		 << ".addSizeChangeCallback(this, std::bind(&"
		 << _name << "_" << classIndex << "::adjust<" << _name << "_" << classIndex
		 << ">, &_" << entry.key << ", this, std::placeholders::_1));\n";
}

void QmlSettingsGenerator::writeSource(const SettingsType &settings)
{
	_src << "#include \"" << _hdrFile.fileName() << "\"\n\n";

	writeQmlRegistration(settings);

	if(settings.qml) {
		const auto &qml = settings.qml.value();
		_src << "\nvoid " << _name << "::registerQmlTypes()\n"
			 << "{\n"
			 << "\tregisterQmlTypes(\"" << qml.uri << "\", " << qml.major << ", " << qml.minor << ");\n"
			 << "}\n";
	}
}

void QmlSettingsGenerator::writeQmlRegistration(const SettingsType &settings)
{
	_src << "\nvoid " << _name << "::registerQmlTypes(const char *uri, int major, int minor)\n"
		 << "{\n"
		 << "}\n";
}

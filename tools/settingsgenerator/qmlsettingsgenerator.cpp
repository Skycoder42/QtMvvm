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
	fixTrContext(settings, QFileInfo{inPath}.fileName());

	_cppName = settings.name.value();
	_name = QStringLiteral("QMLTYPE_") + _cppName;
	_prefixName = settings.prefix ? settings.prefix.value() + QLatin1Char(' ') + _name : _name;
	_typeMappings = settings.typeMappings;

	if(!_hdrFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throw FileException{_hdrFile};
	auto typeNum = writeHeader(settings, QFileInfo{inPath}.completeBaseName() + QStringLiteral(".h"));
	_hdr.flush();
	_hdrFile.close();

	if(!_srcFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throw FileException{_srcFile};
	writeSource(settings, typeNum);
	_src.flush();
	_srcFile.close();
}

int QmlSettingsGenerator::writeHeader(const SettingsType &settings, const QString &inHdrPath)
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
	_hdr << "class " << (settings.prefix ? settings.prefix.value() + QLatin1Char(' ') : QString{}) << _name << " : public QObject\n"
		 << "{\n"
		 << "\tQ_OBJECT\n\n"
		 << "\tusing SelfType = " << _name << ";\n"
		 << "\t" << _cppName << " *_settings;\n"
		 << "\tQList<int> _indexMap;\n\n"
		 << "\tQ_PROPERTY(QtMvvm::ISettingsAccessor *accessor READ accessor CONSTANT FINAL)\n\n";

	QList<QPair<QString, int>> childConstructs;
	QList<int> listEntries;
	writeProperties(settings, keyList, childOffsets, listEntries, childConstructs);

	_hdr << "public:\n"
		 << "\texplicit " << _name << "(" << _cppName << " *settings, QObject *parent = nullptr) : \n"
		 << "\t\tQObject{parent}\n"
		 << "\t\t,_settings{settings}\n";
	writeMemberInits(keyList, childConstructs);
	_hdr << "\t{\n";
	writeEntryPropertySignalConnects(settings, keyList, -1, listEntries);
	_hdr << "\t}\n\n"
		 << "\texplicit " << _name << "(QObject *parent = nullptr) :\n"
		 << "\t\t" << _name << "{" << _cppName << "::instance(), parent}\n"
		 << "\t{}\n\n"
		 << "\tQtMvvm::ISettingsAccessor *accessor() const { return _settings->accessor(); }\n"
		 << "\t" << _cppName << " *settings() const { return _settings; }\n\n"
		 << "\tstatic void registerQmlTypes(const char *uri, int major, int minor);\n";
	if(settings.qml)
		_hdr << "\tstatic void registerQmlTypes();\n";
	_hdr << "};\n\n"
		 << "#endif //" << incGuard << '\n';

	return offset;
}

void QmlSettingsGenerator::writeListTypeBaseClass()
{
	// write the generic variant
	_hdr << "template <typename TList, typename TListParent, typename TNodeValue>\n"
		 << "struct " << _name << "_ListData\n"
		 << "{\n"
		 << "\tTNodeValue &node;\n"
		 << "\tQList<TList*> elements;\n\n"

		 << "\tstatic void adjust(" << _name << "_ListData<TList, TListParent, TNodeValue> *data, QObject *parent, int size) {\n"
		 << "\t\twhile(data->elements.size() > size)\n"
		 << "\t\t\tdata->elements.takeLast()->deleteLater();\n"
		 << "\t\tfor(auto index = data->elements.size(); index < size; index++)\n"
		 << "\t\t\tdata->elements.append(new TList{static_cast<TListParent*>(parent)->_settings, static_cast<TListParent*>(parent)->_indexMap, index, parent});\n"
		 << "\t}\n\n"

		 << "\tstatic void append(QQmlListProperty<TList> *list, TList *element) {\n"
		 << "\t\tconst auto data = reinterpret_cast<" << _name << "_ListData<TList, TListParent, TNodeValue>*>(list->data);\n"
		 << "\t\tconst auto maxIndex = data->node.size();\n"
		 << "\t\tadjust(data, list->object, maxIndex);\n"
		 << "\t\telement->setParent(list->object);\n"
		 << "\t\tdata->elements.append(element);\n"
		 << "\t\tdata->node.commit(maxIndex);\n"
		 << "\t}\n\n"

		 << "\tstatic TList *at(QQmlListProperty<TList> *list, int index) {\n"
		 << "\t\tconst auto data = reinterpret_cast<" << _name << "_ListData<TList, TListParent, TNodeValue>*>(list->data);\n"
		 << "\t\treturn data->elements.size() > index ? data->elements.value(index) : nullptr;\n"
		 << "\t}\n\n"

		 << "\tstatic int count(QQmlListProperty<TList> *list) {\n"
		 << "\t\treturn reinterpret_cast<" << _name << "_ListData<TList, TListParent, TNodeValue>*>(list->data)->elements.size();\n"
		 << "\t}\n\n"

		 << "\tstatic void clear(QQmlListProperty<TList> *list) {\n"
		 << "\t\tconst auto data = reinterpret_cast<" << _name << "_ListData<TList, TListParent, TNodeValue>*>(list->data);\n"
		 << "\t\tfor(auto elem : qAsConst(data->elements))\n"
		 << "\t\t\telem->deleteLater();\n"
		 << "\t\tdata->elements.clear();\n"
		 << "\t\tdata->node.reset();\n"
		 << "\t}\n\n"

		 << "};\n\n";
}

std::tuple<int, QList<int>> QmlSettingsGenerator::writeNodeContentClasses(const NodeContentGroup &node, const QStringList &keyList, int offset, int listDepth)
{
	QList<int> offsetList;
	for(const auto &cNode : node.contentNodes) {
		if(nonstd::holds_alternative<NodeType>(cNode)) {
			offset = writeNodeClass(nonstd::get<NodeType>(cNode), keyList, offset, listDepth);
			offsetList.append(offset - 1);
		} else if(nonstd::holds_alternative<EntryType>(cNode)) {
			if(!nonstd::get<EntryType>(cNode).contentNodes.isEmpty()) {
				offset = writeNodeClass(nonstd::get<EntryType>(cNode), keyList, offset, listDepth);
				offsetList.append(offset - 1);
			} else
				offsetList.append(-1);
		} else if(nonstd::holds_alternative<ListNodeType>(cNode)) {
			offset = writeListNodeClass(nonstd::get<ListNodeType>(cNode), keyList, offset, listDepth);
			offsetList.append(offset - 1);
		} else if(nonstd::holds_alternative<NodeContentGroup>(cNode)) {
			QList<int> subList;
			std::tie(offset, subList) = writeNodeContentClasses(nonstd::get<NodeContentGroup>(cNode), keyList, offset, listDepth);
			offsetList.append(subList);
		} else
			Q_UNREACHABLE();
	}
	return std::make_tuple(offset, offsetList);
}

int QmlSettingsGenerator::writeNodeClass(const NodeType &node, QStringList keyList, int offset, int listDepth)
{
	keyList.append(node.key);
	QList<int> childOffsets;
	std::tie(offset, childOffsets) = writeNodeContentClasses(node, keyList, offset, listDepth);

	_hdr << "class " << _name << "_" << offset << " : public QObject // " << keyList.join(QLatin1Char('/')) << "\n"
		 << "{\n"
		 << "\tQ_OBJECT\n\n"
		 << "\tusing SelfType = " << _name << "_" << offset << ";\n"
		 << "\t" << _cppName << " *_settings;\n"
		 << "\tconst QList<int> &_indexMap;\n\n";

	QList<QPair<QString, int>> childConstructs;
	QList<int> listEntries;
	writeProperties(node, keyList, childOffsets, listEntries, childConstructs);

	_hdr << "public:\n"
		 << "\t" << _name << "_" << offset << "(" << _cppName << " *settings, const QList<int> &indexMap, QObject *parent) : \n"
		 << "\t\tQObject{parent}\n"
		 << "\t\t,_settings{settings}\n"
		 << "\t\t,_indexMap{indexMap}\n";
	writeMemberInits(keyList, childConstructs);
	_hdr << "\t{\n";
	writeEntryPropertySignalConnects(node, keyList, offset, listEntries);
	_hdr << "\t}\n"
		 << "};\n\n";

	return ++offset;
}

int QmlSettingsGenerator::writeListNodeClass(const ListNodeType &node, QStringList keyList, int offset, int listDepth)
{
	keyList.append({node.key, QStringLiteral("at(_indexMap.at(%1))").arg(listDepth++)});
	QList<int> childOffsets;
	std::tie(offset, childOffsets) = writeNodeContentClasses(node, keyList, offset, listDepth);

	_hdr << "class " << _name << "_" << offset << " : public QObject // " << keyList.join(QLatin1Char('/')) << "\n"
		 << "{\n"
		 << "\tQ_OBJECT\n\n"
		 << "\tusing SelfType = " << _name << "_" << offset << ";\n"
		 << "\t" << _cppName << " *_settings;\n"
		 << "\tQList<int> _indexMap;\n\n";

	QList<QPair<QString, int>> childConstructs;
	QList<int> listEntries;
	writeProperties(node, keyList, childOffsets, listEntries, childConstructs);

	_hdr << "public:\n"
		 << "\t" << _name << "_" << offset << "(" << _cppName << " *settings, const QList<int> &indexMap, int index, QObject *parent) : \n"
		 << "\t\tQObject{parent}\n"
		 << "\t\t,_settings{settings}\n"
		 << "\t\t,_indexMap{QList<int>{indexMap} << index}\n";
	writeMemberInits(keyList, childConstructs);
	_hdr << "\t{\n";
	writeEntryPropertySignalConnects(node, keyList, offset, listEntries);
	_hdr << "\t}\n"
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
		else if(nonstd::holds_alternative<ListNodeType>(cNode)) {
			listEntries.append(childOffsets.first());
			writeListNodeProperty(nonstd::get<ListNodeType>(cNode), keyList, childOffsets.takeFirst(), childConstructs);
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
			 << " NOTIFY " << entry.key << "Changed)\n";

		_hdr << "\t" << mType << " get_" << entry.key << "() const { return _settings->" << keyList.join(QLatin1Char('.')) << ".get(); }\n"
			 << "\tvoid set_" << entry.key << "(const " << mType << " &value) { _settings->" << keyList.join(QLatin1Char('.')) << ".set(value); }\n"
			 << "Q_SIGNALS:\n"
			 << "\tvoid " << entry.key << "Changed(const " << mType << " &value);\n"
			 << "private:\n\n";
	}

	if(!entry.contentNodes.isEmpty())
		writeNodeProperty(entry, classIndex, childConstructs, entry.qmlGroupKey.value_or(entry.key + QStringLiteral("Group")));
}

void QmlSettingsGenerator::writeListNodeProperty(const ListNodeType &node, QStringList keyList, int classIndex, QList<QPair<QString, int>> &childConstructs)
{
	keyList.append(node.key);
	_hdr << "\tusing ListData_" << classIndex << " = " << _name << "_ListData<" << _name << "_" << classIndex << ", SelfType, typename std::decay<decltype(_settings->" << keyList.join(QLatin1Char('.')) << ")>::type>;\n"
		 << "\tfriend ListData_" << classIndex << ";\n"
		 << "\tQ_PROPERTY(QQmlListProperty<" << _name << "_" << classIndex << "> " << node.key
		 << " READ get_" << node.key << " CONSTANT)\n"
		 << "\tListData_" << classIndex << " _" << node.key << ";\n"
		 << "\tQQmlListProperty<" << _name << "_" << classIndex << "> get_" << node.key << "() {\n"
		 << "\t\treturn {\n"
		 << "\t\t\tthis, &_" << node.key << ",\n"
		 << "\t\t\t&ListData_" << classIndex << "::append,\n"
		 << "\t\t\t&ListData_" << classIndex << "::count,\n"
		 << "\t\t\t&ListData_" << classIndex << "::at,\n"
		 << "\t\t\t&ListData_" << classIndex << "::clear\n"
		 << "\t\t};\n"
		 << "\t}\n"
		 << "public:\n"
		 << "\tQ_INVOKABLE " << _name << "_" << classIndex << " *" << node.key << "_push_deferred() {\n"
		 << "\t\treturn new " << _name << "_" << classIndex << "{_settings, _indexMap, _" << node.key << ".node.size(), this};\n"
		 << "\t}\n"
		 << "\tQ_INVOKABLE " << _name << "_" << classIndex << " *" << node.key << "_push() {\n"
		 << "\t\tauto data = " << node.key << "_push_deferred();\n"
		 << "\t\tauto qmlList = get_" << node.key << "();\n"
		 << "\t\tListData_" << classIndex << "::append(&qmlList, data);\n"
		 << "\t\treturn data;\n"
		 << "\t}\n"
		 << "private:\n\n";

	childConstructs.append({node.key, -1});
}

void QmlSettingsGenerator::writeMemberInits(const QStringList &keyList, const QList<QPair<QString, int>> &childConstructs)
{
	for(const auto &info : childConstructs) {
		if(info.second < 0)
			_hdr << "\t\t,_" << info.first << "{_settings->" << (QStringList{keyList} << info.first).join(QLatin1Char('.')) << ", {}}\n";
		else
			_hdr << "\t\t,_" << info.first << "{new " << _name << "_" << info.second << "{_settings, _indexMap, this}}\n";
	}
}

void QmlSettingsGenerator::writeEntryPropertySignalConnects(const NodeContentGroup &node, const QStringList &keyList, int classIndex, QList<int> &listEntries)
{
	for(const auto &cNode : node.contentNodes) {
		if(nonstd::holds_alternative<NodeType>(cNode))
			continue; //nothing to be set
		else if(nonstd::holds_alternative<EntryType>(cNode))
			writeEntryPropertySignalConnect(nonstd::get<EntryType>(cNode), keyList, classIndex);
		else if(nonstd::holds_alternative<ListNodeType>(cNode))
			writeListNodePropertySignalConnect(nonstd::get<ListNodeType>(cNode), keyList, listEntries);
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
	_hdr << "::" << entry.key << "Changed, this, std::placeholders::_1));\n";
}

void QmlSettingsGenerator::writeListNodePropertySignalConnect(const ListNodeType &entry, QStringList keyList, QList<int> &listEntries)
{
	auto classIndex = listEntries.takeFirst();
	keyList.append(entry.key);
	_hdr << "\t\t_settings->" << keyList.join(QLatin1Char('.'))
		 << ".addChangeCallback(this, std::bind(&"
		 << "ListData_" << classIndex << "::adjust, &_" << entry.key << ", this, std::placeholders::_1));\n"
		 << "\t\t" << "ListData_" << classIndex << "::adjust"
		 << "(&_" << entry.key << ", this, _settings->" << keyList.join(QLatin1Char('.')) << ".size());\n";
}

void QmlSettingsGenerator::writeSource(const SettingsType &settings, int typeNum)
{
	_src << "#include \"" << _hdrFile.fileName() << "\"\n"
		 << "#include <QtCore/QCoreApplication>\n"
		 << "#include <QtQml/QQmlEngine>\n\n";

	writeQmlRegistration(settings.qml ? settings.qml.value().type : Singleton, typeNum);

	if(settings.qml) {
		const auto &qml = settings.qml.value();
		_src << "void " << _name << "::registerQmlTypes()\n"
			 << "{\n"
			 << "\tregisterQmlTypes(\"" << qml.uri << "\", " << qml.major << ", " << qml.minor << ");\n"
			 << "}\n";

		if(qml.autoRegister) {
			_src << "\nnamespace {\n\n"
				 << "void __register_qml_types()\n"
				 << "{\n"
				 << "\t" << _name << "::registerQmlTypes();\n"
				 << "}\n\n"
				 << "}\n"
				 << "Q_COREAPP_STARTUP_FUNCTION(__register_qml_types)\n";
		}
	}
}

void QmlSettingsGenerator::writeQmlRegistration(QmlRegistrationMode mode, int typeNum)
{
	if(mode == Singleton) {
		_src << "namespace {\n\n"
			 << "QObject *__create_qml_singleton(QQmlEngine *qmlEngine, QJSEngine *)\n"
			 << "{\n"
			 << "\treturn new " << _name << "{qmlEngine};\n"
			 << "}\n\n"
			 << "}\n\n";
	}

	_src << "void " << _name << "::registerQmlTypes(const char *uri, int major, int minor)\n"
		 << "{\n"
		 << "\tconst QString msg{QStringLiteral(\"Settings-Helpertypes cannot be created\")};\n\n";
	for(auto i = 0; i < typeNum; i++)
		_src << "\tqmlRegisterUncreatableType<" << _name << "_" << i << ">(uri, major, minor, \"" << _name << "_" << i << "\", msg);\n";

	switch(mode) {
	case Singleton:
		_src << "\n\tqmlRegisterSingletonType<" << _name << ">(uri, major, minor, \"" << _cppName << "\", __create_qml_singleton);\n";
		break;
	case Uncreatable:
		_src << "\n\tqmlRegisterUncreatableType<" << _name << ">(uri, major, minor, \"" << _cppName << "\", msg);\n";
		break;
	case Creatable:
		_src << "\n\tqmlRegisterType<" << _name << ">(uri, major, minor, \"" << _cppName << "\");\n";
		break;
	default:
		Q_UNREACHABLE();
		break;
	}
	_src << "}\n\n";
}

#include "settingsgeneratorimpl.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>

bool SettingsGeneratorImpl::read_type_mapping(QXmlStreamReader &reader, QHash<QString, QString> &data, bool hasNext)
{
	TypeMappingGroup grp;
	hasNext = read_TypeMappingGroup(reader, grp, hasNext);
	for(const auto &mapping : qAsConst(grp.typeMapping))
		data.insert(mapping.key, mapping.type);
	return hasNext;
}

void SettingsGeneratorImpl::read_included_file(QXmlStreamReader &reader, NodeContentGroup &data)
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
		QFile xmlFile{import.importPath};
		if(!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text))
			throw FileException{xmlFile};

		QXmlStreamReader subReader{&xmlFile};
		if(!subReader.readNextStartElement())
			throw XmlException{subReader};

		SettingsType settings;
		if(subReader.name() == QStringLiteral("Settings")) {
			read_SettingsType(subReader, settings);
		} else if(subReader.name() == QStringLiteral("SettingsConfig")) {
			SettingsConfigImpl confReader;
			SettingsConfigBase::SettingsConfigType settingsConf;
			confReader.read_SettingsConfigType(subReader, settingsConf);
			convertFromConf(reader, settingsConf, settings);
		} else
			throwChild(subReader);

		NodeContentGroup *cGrp = &settings;
		if(import.rootNode) {
			for(const auto &key : import.rootNode.value().split(QLatin1Char('/'), QString::SkipEmptyParts)) {
				cGrp = findContentGroup(cGrp, key);
				if(!cGrp)
					return;
			}
		}

		data = std::move(*cGrp);
		fixTrContext(data, QFileInfo{import.importPath}.fileName());
	} catch(FileException &e) {
		if(import.required)
			throw;
		else {
			qWarning() << e.what();
			return;
		}
	}
}

void SettingsGeneratorImpl::convertFromConf(QXmlStreamReader &reader, SettingsConfigBase::SettingsConfigType &conf, SettingsType &data)
{
	for(auto &element : conf.content) {
		if(nonstd::holds_alternative<SettingsConfigBase::CategoryType>(element))
			readCategory(reader, nonstd::get<SettingsConfigBase::CategoryType>(element), data);
		else if(nonstd::holds_alternative<SettingsConfigBase::SectionType>(element))
			readSection(reader, nonstd::get<SettingsConfigBase::SectionType>(element), data);
		else if(nonstd::holds_alternative<SettingsConfigBase::GroupType>(element))
			readGroup(reader, nonstd::get<SettingsConfigBase::GroupType>(element), data);
		else if(nonstd::holds_alternative<SettingsConfigBase::EntryType>(element))
			readEntry(reader, nonstd::get<SettingsConfigBase::EntryType>(element), data);
		else
			throw XmlException{reader, QStringLiteral("Unexpected child element in included SettingsConf")};
	}
}

void SettingsGeneratorImpl::readCategory(QXmlStreamReader &reader, SettingsConfigBase::CategoryContentGroup &content, NodeContentGroup &targetRootNode)
{
	for(auto &element : content.content) {
		if(nonstd::holds_alternative<SettingsConfigBase::SectionType>(element))
			readSection(reader, nonstd::get<SettingsConfigBase::SectionType>(element), targetRootNode);
		else if(nonstd::holds_alternative<SettingsConfigBase::GroupType>(element))
			readGroup(reader, nonstd::get<SettingsConfigBase::GroupType>(element), targetRootNode);
		else if(nonstd::holds_alternative<SettingsConfigBase::EntryType>(element))
			readEntry(reader, nonstd::get<SettingsConfigBase::EntryType>(element), targetRootNode);
		else
			throw XmlException{reader, QStringLiteral("Unexpected child element in included SettingsConf Categoy")};
	}
}

void SettingsGeneratorImpl::readSection(QXmlStreamReader &reader, SettingsConfigBase::SectionContentGroup &content, NodeContentGroup &targetRootNode)
{
	for(auto &element : content.content) {
		if(nonstd::holds_alternative<SettingsConfigBase::GroupType>(element))
			readGroup(reader, nonstd::get<SettingsConfigBase::GroupType>(element), targetRootNode);
		else if(nonstd::holds_alternative<SettingsConfigBase::EntryType>(element))
			readEntry(reader, nonstd::get<SettingsConfigBase::EntryType>(element), targetRootNode);
		else
			throw XmlException{reader, QStringLiteral("Unexpected child element in included SettingsConf Section")};
	}
}

void SettingsGeneratorImpl::readGroup(QXmlStreamReader &reader, SettingsConfigBase::GroupContentGroup &content, NodeContentGroup &targetRootNode)
{
	for(auto &element : content.content) {
		if(nonstd::holds_alternative<SettingsConfigBase::EntryType>(element))
			readEntry(reader, nonstd::get<SettingsConfigBase::EntryType>(element), targetRootNode);
		else
			throw XmlException{reader, QStringLiteral("Unexpected child element in included SettingsConf Group")};
	}
}

void SettingsGeneratorImpl::readEntry(QXmlStreamReader &reader, SettingsConfigBase::EntryType &entry, NodeContentGroup &targetRootNode)
{
	auto keyChain = entry.key.split(QLatin1Char('/'), QString::SkipEmptyParts);
	auto entryKey = keyChain.takeLast();

	auto cGrp = &targetRootNode;
	for(const auto &key : keyChain) {
		auto nGrp = findContentGroup(cGrp, key);
		if(!nGrp) {
			NodeType nNode;
			nNode.key = key;
			cGrp->contentNodes.append(std::move(nNode));
			nGrp = &(nonstd::get<NodeType>(cGrp->contentNodes.last()));
		}
		cGrp = nGrp;
	}

	auto isEntry = false;
	auto eGrp = findContentGroup(cGrp, entryKey, &isEntry);
	if(!eGrp) {
		EntryType nEntry;
		nEntry.key = entryKey;
		cGrp->contentNodes.append(std::move(nEntry));
		eGrp = &(nonstd::get<EntryType>(cGrp->contentNodes.last()));
	} else if(!isEntry) {
		EntryType nEntry;
		nEntry.key = entryKey;
		nEntry.contentNodes = std::move(eGrp->contentNodes);
		eGrp = replaceNodeByEntry(cGrp, eGrp, nEntry);
		Q_ASSERT(eGrp);
	} else
		throw XmlException{reader, QStringLiteral("Found duplicated entry with key: %1").arg(entry.key)};

	auto nEntry = static_cast<EntryType*>(eGrp);
	nEntry->type = std::move(entry.type);
	nEntry->defaultValue = std::move(entry.defaultValue);
	nEntry->tr = entry.trdefault;
}

SettingsGeneratorBase::NodeContentGroup *SettingsGeneratorImpl::findContentGroup(SettingsGeneratorBase::NodeContentGroup *cGrp, const QString &key, bool *isEntry)
{
	if(isEntry)
		*isEntry = false;
	for(auto &cNode : cGrp->contentNodes) {
		if(nonstd::holds_alternative<NodeType>(cNode)) {
			if(nonstd::get<NodeType>(cNode).key == key)
				return &(nonstd::get<NodeType>(cNode));
		} else if(nonstd::holds_alternative<EntryType>(cNode)) {
			if(nonstd::get<EntryType>(cNode).key == key) {
				if(isEntry)
					*isEntry = true;
				return &(nonstd::get<EntryType>(cNode));
			}
		} else if(nonstd::holds_alternative<ListNodeType>(cNode)) {
			if(nonstd::get<ListNodeType>(cNode).key == key)
				return &(nonstd::get<ListNodeType>(cNode));
		} else if(nonstd::holds_alternative<NodeContentGroup>(cNode)) {
			auto res = findContentGroup(&(nonstd::get<NodeContentGroup>(cNode)), key);
			if(res)
				return res;
		}
	}

	return nullptr;
}

SettingsGeneratorBase::NodeContentGroup *SettingsGeneratorImpl::replaceNodeByEntry(SettingsGeneratorBase::NodeContentGroup *cGrp, NodeContentGroup *node, SettingsGeneratorBase::EntryType &entry)
{
	for(auto &cNode : cGrp->contentNodes) {
		if(nonstd::holds_alternative<NodeType>(cNode)) {
			if(&nonstd::get<NodeType>(cNode) == node) {
				cNode = std::move(entry);
				return &(nonstd::get<EntryType>(cNode));
			}
		} else if(nonstd::holds_alternative<NodeContentGroup>(cNode)) {
			auto res = replaceNodeByEntry(&(nonstd::get<NodeContentGroup>(cNode)), node, entry);
			if(res)
				return res;
		}
	}

	return nullptr;
}

void SettingsGeneratorImpl::fixTrContext(NodeContentGroup &group, const QString &context)
{
	for(auto &node : group.contentNodes) {
		if(nonstd::holds_alternative<NodeType>(node))
			fixTrContext(nonstd::get<NodeType>(node), context);
		else if(nonstd::holds_alternative<EntryType>(node)) {
			auto &entry = nonstd::get<EntryType>(node);
			if(!entry.trContext)
				entry.trContext = context;
			fixTrContext(entry, context);
		} else if(nonstd::holds_alternative<ListNodeType>(node)) {
			fixTrContext(nonstd::get<ListNodeType>(node), context);
		} else if(nonstd::holds_alternative<NodeContentGroup>(node))
			fixTrContext(nonstd::get<NodeContentGroup>(node), context);
	}
}

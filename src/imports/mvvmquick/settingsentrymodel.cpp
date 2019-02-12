#include "settingsentrymodel.h"

#include <QtCore/QRegularExpression>
#include <QtMvvmCore/CoreApp>

using namespace QtMvvm;

const QList<int> SettingsEntryModel::FilterRoles {
	SettingsEntryModel::GroupRole,
	SettingsEntryModel::TitleRole,
	SettingsEntryModel::ToolTipRole,
	SettingsEntryModel::SearchKeysRole
};

SettingsEntryModel::SettingsEntryModel(QObject *parent) :
	QAbstractListModel{parent}
{}

void SettingsEntryModel::setup(const SettingsElements::Section &section, SettingsViewModel *viewModel, InputViewFactory *factory)
{
	beginResetModel();
	_entries.clear();
	if(_viewModel) {
		disconnect(_viewModel, &SettingsViewModel::valueChanged,
				   this, &SettingsEntryModel::entryChanged);
	}
	_viewModel = viewModel;
	_factory = factory;
	connect(_viewModel, &SettingsViewModel::valueChanged,
			this, &SettingsEntryModel::entryChanged,
			Qt::QueuedConnection); //to not mess up data changes
	auto rIndex = 0;
	for(const auto &group : section.groups) {
		for(const auto &entry : group.entries) {
			auto url = _factory->getDelegate(entry.type, entry.properties);
			if(group.title.isEmpty()) // unnamed groups are presented first
				_entries.insert(rIndex++, EntryInfo{entry, url});
			else
				_entries.append(EntryInfo{entry, url, group});
		}
	}
	endResetModel();
}

int SettingsEntryModel::rowCount(const QModelIndex &parent) const
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
	if(!checkIndex(parent, QAbstractItemModel::CheckIndexOption::ParentIsInvalid))
		return 0;
#endif
	if (parent.isValid())
		return 0;
	else
		return _entries.size();
}

QVariant SettingsEntryModel::data(const QModelIndex &index, int role) const
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
	if(!checkIndex(index, QAbstractItemModel::CheckIndexOption::ParentIsInvalid | QAbstractItemModel::CheckIndexOption::IndexIsValid))
		return {};
#else
	if(!index.isValid())
		return {};
#endif

	auto entry = _entries.value(index.row());
	switch (role) {
	case Qt::DisplayRole:
	case TitleRole:
		return entry.title;
	case KeyRole:
		return entry.key;
	case TypeRole:
		return QString::fromUtf8(entry.type);
	case ToolTipRole:
		return entry.tooltip;
	case DelegateUrlRole:
		return entry.delegateUrl;
	case SettingsValueRole:
		return readValue(entry);
	case PropertiesRole:
		return entry.properties;
	case GroupRole:
		return entry.group.title;
	case SearchKeysRole:
		return entry.searchKeys;
	case PreviewRole:
		if(entry.properties.contains(QStringLiteral("qtmvvm_preview"))) {
			auto preview = entry.properties.value(QStringLiteral("qtmvvm_preview"));
			if(preview.type() == QVariant::Bool)
				return preview.toBool() ? entry.tooltip : QString{};
			else if(preview.type() == QVariant::String) {
				return _factory->format(entry.type,
										preview.toString(),
										readValue(entry),
										entry.properties);
			}
		}
		return QString{};
	default:
		return QVariant();
	}
}

bool SettingsEntryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
	if(!checkIndex(index, QAbstractItemModel::CheckIndexOption::ParentIsInvalid | QAbstractItemModel::CheckIndexOption::IndexIsValid))
		return false;
#else
	if(!index.isValid())
		return false;
#endif
	if(role != SettingsValueRole)
		return false;

	_viewModel->saveValue(_entries.value(index.row()).key, value);
	emit dataChanged(index, index, {SettingsValueRole, PreviewRole});
	return true;
}

QHash<int, QByteArray> SettingsEntryModel::roleNames() const
{
	return {
		{GroupRole, "group"},
		{KeyRole, "key"},
		{TypeRole, "type"},
		{TitleRole, "title"},
		{ToolTipRole, "tooltip"},
		{DelegateUrlRole, "delegateUrl"},
		{SettingsValueRole, "inputValue"},
		{PropertiesRole, "properties"},
		{PreviewRole, "preview"}
	};
}

Qt::ItemFlags SettingsEntryModel::flags(const QModelIndex &index) const
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
	if(!checkIndex(index, QAbstractItemModel::CheckIndexOption::ParentIsInvalid))
		return Qt::NoItemFlags;
#endif
	return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

void SettingsEntryModel::entryChanged(const QString &key)
{
	for(auto i = 0; i < _entries.size(); i++) {
		if(_entries[i].key == key) {
			auto mIndex = index(i);
			emit dataChanged(mIndex, mIndex, {SettingsValueRole, PreviewRole});
			break;
		}
	}
}

QVariant SettingsEntryModel::readValue(const SettingsElements::Entry &entry) const
{
	return CoreApp::safeCastInputType(entry.type, _viewModel->loadValue(entry.key, entry.defaultValue));
}



SettingsEntryModel::EntryInfo::EntryInfo(SettingsElements::Entry entry, QUrl delegateUrl, SettingsElements::Group group) :
	Entry(std::move(entry)),
	delegateUrl(std::move(delegateUrl)),
	group(std::move(group))
{
	static const QRegularExpression nameRegex(QStringLiteral("&(?!&)"),
											  QRegularExpression::DontCaptureOption);
	title.remove(nameRegex);
	this->group.entries.clear();
}

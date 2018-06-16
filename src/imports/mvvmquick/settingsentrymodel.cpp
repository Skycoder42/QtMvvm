#include "settingsentrymodel.h"

#include <QtCore/QRegularExpression>

using namespace QtMvvm;

const QList<int> SettingsEntryModel::FilterRoles {
	SettingsEntryModel::GroupRole,
	SettingsEntryModel::TitleRole,
	SettingsEntryModel::ToolTipRole,
	SettingsEntryModel::SearchKeysRole
};

SettingsEntryModel::SettingsEntryModel(QObject *parent) :
	QAbstractListModel(parent),
	_entries()
{}

void SettingsEntryModel::setup(const SettingsElements::Section &section, SettingsViewModel *viewModel, InputViewFactory *factory)
{
	beginResetModel();
	_entries.clear();
	_viewModel = viewModel;
	auto rIndex = 0;
	for(const auto &group : section.groups) {
		for(const auto &entry : group.entries) {
			auto url = factory->getDelegate(entry.type, entry.properties);
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
	if (parent.isValid())
		return 0;
	else
		return _entries.size();
}

QVariant SettingsEntryModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

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
		return _viewModel->loadValue(entry.key, entry.defaultValue);
	case PropertiesRole:
		return entry.properties;
	case GroupRole:
		return entry.group.title;
	case SearchKeysRole:
		return entry.searchKeys;
	default:
		return QVariant();
	}
}

bool SettingsEntryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid() || role != SettingsValueRole)
		return false;

	_viewModel->saveValue(_entries.value(index.row()).key, value);
	emit dataChanged(index, index, {SettingsValueRole});
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
		{PropertiesRole, "properties"}
	};
}

Qt::ItemFlags SettingsEntryModel::flags(const QModelIndex &index) const
{
	return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}



SettingsEntryModel::EntryInfo::EntryInfo(SettingsElements::Entry entry, QUrl delegateUrl, SettingsElements::Group group) :
	Entry(std::move(entry)),
	delegateUrl(std::move(delegateUrl)),
	group(std::move(group))
{
	static const QRegularExpression nameRegex(QStringLiteral("&(?!&)"),
											  QRegularExpression::DontCaptureOption |
											  QRegularExpression::OptimizeOnFirstUsageOption);
	title.remove(nameRegex);
	this->group.entries.clear();
}

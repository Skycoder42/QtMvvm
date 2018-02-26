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
	for(auto group : section.groups) {
		for(auto entry : group.entries) {
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

	switch (role) {
	case Qt::DisplayRole:
	case TitleRole:
		return _entries.value(index.row()).title;
	case KeyRole:
		return _entries.value(index.row()).key;
	case TypeRole:
		return QString::fromUtf8(_entries.value(index.row()).type);
	case ToolTipRole:
		return _entries.value(index.row()).tooltip;
	case DelegateUrlRole:
		return _entries.value(index.row()).delegateUrl;
	case SettingsValueRole:
		return _viewModel->loadValue(_entries.value(index.row()).key);
	case PropertiesRole:
		return _entries.value(index.row()).properties;
	case GroupRole:
		return _entries.value(index.row()).group.title;
	case SearchKeysRole:
		return _entries.value(index.row()).searchKeys;
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
		{SettingsValueRole, "settingsValue"},
		{PropertiesRole, "properties"}
	};
}

Qt::ItemFlags SettingsEntryModel::flags(const QModelIndex &index) const
{
	return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}



SettingsEntryModel::EntryInfo::EntryInfo(SettingsElements::Entry entry, const QUrl &delegateUrl, SettingsElements::Group group) :
	Entry(entry),
	delegateUrl(delegateUrl),
	group(group)
{
	static const QRegularExpression nameRegex(QStringLiteral("&(?!&)"),
											  QRegularExpression::DontCaptureOption |
											  QRegularExpression::OptimizeOnFirstUsageOption);
	title.remove(nameRegex);
	group.entries.clear();
}

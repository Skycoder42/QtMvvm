#include "settingssectionmodel.h"
#include "settingsuibuilder.h"
using namespace QtMvvm;

const QList<int> SettingsSectionModel::FilterRoles {
	SettingsSectionModel::CategoryRole,
	SettingsSectionModel::TitleRole,
	SettingsSectionModel::ToolTipRole,
	SettingsSectionModel::ExtraSearchKeysRole
};

SettingsSectionModel::SettingsSectionModel(QObject *parent) :
	QAbstractListModel(parent),
	_sections(),
	_hasSections(false)
{}

void SettingsSectionModel::setup(const SettingsElements::Setup &setup)
{
	beginResetModel();
	_sections.clear();
	_hasSections = false;
	auto rIndex = 0;
	for(const auto &category : setup.categories) {
		if(category.sections.size() == 1) { // single sects are always at the beginning
			_hasSections = true;
			_sections.insert(rIndex++, category);
		} else {
			for(const auto &section : category.sections)
				_sections.append(SectionInfo{section, category});
		}
	}
	endResetModel();
}

int SettingsSectionModel::rowCount(const QModelIndex &parent) const
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
	if(!checkIndex(parent, QAbstractItemModel::CheckIndexOption::ParentIsInvalid))
		return 0;
#endif
	if (parent.isValid())
		return 0;
	else
		return _sections.size();
}

QVariant SettingsSectionModel::data(const QModelIndex &index, int role) const
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
	if(!checkIndex(index, QAbstractItemModel::CheckIndexOption::ParentIsInvalid | QAbstractItemModel::CheckIndexOption::IndexIsValid))
		return {};
#else
	if (!index.isValid())
		return {};
#endif

	switch (role) {
	case Qt::DisplayRole:
	case TitleRole:
		return _sections.value(index.row()).title;
	case IconRole:
		return _sections.value(index.row()).icon;
	case ToolTipRole:
		return _sections.value(index.row()).tooltip;
	case CategoryRole:
		return _sections.value(index.row()).category.title;
	case SectionRole:
		return QVariant::fromValue<SettingsElements::Section>(_sections.value(index.row()));
	case ExtraSearchKeysRole:
	{
		const auto &section = _sections[index.row()];
		if(section.searchKeys.isEmpty()) {
			for(const auto &group : section.groups) {
				section.searchKeys.append(group.title);
				for(const auto &entry : group.entries) {
					section.searchKeys.append(entry.title);
					section.searchKeys.append(entry.tooltip);
					section.searchKeys.append(entry.searchKeys);
				}
			}
		}
		return section.searchKeys;
	}
	default:
		return QVariant();
	}
}

QHash<int, QByteArray> SettingsSectionModel::roleNames() const
{
	return {
		{CategoryRole, "category"},
		{TitleRole, "title"},
		{IconRole, "iconUrl"},
		{ToolTipRole, "tooltip"},
		{SectionRole, "section"}
	};
}

bool SettingsSectionModel::hasSections() const
{
	return _hasSections;
}



SettingsSectionModel::SectionInfo::SectionInfo(SettingsElements::Section section, SettingsElements::Category category) :
	Section(std::move(section)),
	category(std::move(category))
{
	this->category.sections.clear();
}

SettingsSectionModel::SectionInfo::SectionInfo(const SettingsElements::Category &category) :
	Section{category.title, category.icon, category.tooltip, category.sections.first().groups, {}, {}},
	category()
{}

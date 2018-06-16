#ifndef QTMVVM_SETTINGSSECTIONMODEL_H
#define QTMVVM_SETTINGSSECTIONMODEL_H

#include <QtCore/QAbstractListModel>

#include <QtMvvmCore/SettingsElements>

namespace QtMvvm {

class SettingsSectionModel : public QAbstractListModel
{
	Q_OBJECT

	Q_PROPERTY(bool hasSections READ hasSections CONSTANT)

public:
	enum Roles {
		CategoryRole = Qt::UserRole + 1,
		TitleRole,
		IconRole,
		ToolTipRole,
		SectionRole,
		ExtraSearchKeysRole
	};
	Q_ENUM(Roles)

	static const QList<int> FilterRoles;

	explicit SettingsSectionModel(QObject *parent = nullptr);

	void setup(const SettingsElements::Setup &setup);

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;

	bool hasSections() const;

private:
	struct SectionInfo : public SettingsElements::Section {
	public:
		SectionInfo(SettingsElements::Section section = {}, SettingsElements::Category category = {});
		SectionInfo(const SettingsElements::Category &category);

		SettingsElements::Category category;
		mutable QStringList searchKeys;
	};

	QList<SectionInfo> _sections;
	bool _hasSections;
};

}

#endif // QTMVVM_SETTINGSSECTIONMODEL_H

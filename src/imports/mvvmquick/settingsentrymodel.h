#ifndef QTMVVM_SETTINGSENTRYMODEL_H
#define QTMVVM_SETTINGSENTRYMODEL_H

#include <QtCore/QAbstractListModel>

#include <QtMvvmCore/SettingsViewModel>
#include <QtMvvmCore/SettingsElements>

#include <QtMvvmQuick/InputViewFactory>

namespace QtMvvm {

class SettingsEntryModel : public QAbstractListModel
{
	Q_OBJECT

public:
	enum Roles {
		GroupRole = Qt::UserRole + 1,
		KeyRole,
		TypeRole,
		TitleRole,
		ToolTipRole,
		DelegateUrlRole,
		SettingsValueRole,
		PropertiesRole,
		SearchKeysRole,

		PreviewRole
	};
	Q_ENUM(Roles)

	static const QList<int> FilterRoles;

	explicit SettingsEntryModel(QObject *parent = nullptr);

	void setup(const SettingsElements::Section &setup, SettingsViewModel *viewModel, InputViewFactory *factory);

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role) override;
	QHash<int, QByteArray> roleNames() const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;

private Q_SLOTS:
	void entryChanged(const QString &key);

private:
	struct EntryInfo : public SettingsElements::Entry {
	public:
		EntryInfo(SettingsElements::Entry entry = {}, QUrl delegateUrl = {}, SettingsElements::Group group = {});

		QUrl delegateUrl;
		SettingsElements::Group group;
	};

	SettingsViewModel *_viewModel = nullptr;
	InputViewFactory *_factory = nullptr;
	QList<EntryInfo> _entries;

	QVariant readValue(const SettingsElements::Entry &entry) const;
};

}

#endif // QTMVVM_SETTINGSENTRYMODEL_H

#ifndef QTMVVM_SETTINGSUIBUILDER_H
#define QTMVVM_SETTINGSUIBUILDER_H

#include <QtCore/QObject>
#include <QtCore/QUrl>

#include <QtMvvmCore/SettingsViewModel>

#include <QtQuick/QQuickItem>

#include "multifilterproxymodel.h"
#include "settingssectionmodel.h"
#include "settingsentrymodel.h"

namespace QtMvvm {

class SettingsUiBuilder : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QQuickItem* buildView MEMBER _buildView NOTIFY buildViewChanged)
	Q_PROPERTY(SettingsViewModel* viewModel MEMBER _viewModel WRITE setViewModel NOTIFY viewModelChanged)
	Q_PROPERTY(QString filterText READ filterText WRITE setFilterText NOTIFY filterTextChanged)
	Q_PROPERTY(bool allowSearch MEMBER _allowSearch  NOTIFY allowSearchChanged)
	Q_PROPERTY(bool allowRestore MEMBER _allowRestore  NOTIFY allowRestoreChanged)

public:
	explicit SettingsUiBuilder(QObject *parent = nullptr);

	QString filterText() const;

public Q_SLOTS:
	void loadSection(const QtMvvm::SettingsElements::Section &section);
	void showDialog(const QString &key, const QString &title, const QString &type, const QVariant &defaultValue, const QVariantMap &properties);

	void restoreDefaults();
	void setFilterText(QString filterText);

Q_SIGNALS:
	void presentOverview(QAbstractItemModel *model, bool hasSections);
	void presentSection(QAbstractItemModel *model);
	void closeSettings();

	void buildViewChanged(QQuickItem* buildView);
	void viewModelChanged(SettingsViewModel* viewModel);
	void filterTextChanged(QString filterText);
	void allowSearchChanged(bool allowSearch);
	void allowRestoreChanged(bool allowRestore);

private Q_SLOTS:
	void startBuildUi();

	void setViewModel(SettingsViewModel *viewModel);

private:
	QQuickItem* _buildView = nullptr;
	SettingsViewModel *_viewModel = nullptr;
	QString _filterText;

	bool _allowSearch = true;
	bool _allowRestore = true;

	MultiFilterProxyModel *_sectionFilterModel;
	SettingsSectionModel *_sectionModel;
	MultiFilterProxyModel *_entryFilterModel;
	SettingsEntryModel *_entryModel;

	SettingsElements::Setup _currentSetup;
};

}

#endif // QTMVVM_SETTINGSUIBUILDER_H

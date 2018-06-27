#ifndef QTMVVM_SETTINGSDIALOG_P_H
#define QTMVVM_SETTINGSDIALOG_P_H

#include <functional>

#include <QtCore/QSet>

#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QAbstractButton>

#include "qtmvvmwidgets_global.h"
#include "settingsdialog.h"

namespace Ui {
class SettingsDialog;
}

namespace QtMvvm {

class CategoryItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	CategoryItemDelegate(std::function<void(int)> updateFunc,
						 QSize iconSize,
						 int layoutSpacing,
						 QObject *parent = nullptr);

	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
	QSize _iconSize;
	std::function<void(int)> _updateFunc;
};

class SettingsDialogPrivate : public QObject
{
	Q_OBJECT

public:
	SettingsDialogPrivate(SettingsDialog *q_ptr, ViewModel *viewModel);
	~SettingsDialogPrivate() override;

	static const QString TabContentId;

	SettingsDialog *q;
	SettingsViewModel *viewModel;
	QScopedPointer<Ui::SettingsDialog> ui;
	CategoryItemDelegate *delegate = nullptr;
	int maxWidthBase = 0;

	SettingsElements::Setup setup;

	using EntryInfo = QPair<SettingsElements::Entry, QMetaProperty>;
	QHash<QWidget*, EntryInfo> entryMap;
	QHash<QString, QWidget*> keyMap;
	QSet<QWidget*> changedEntries;

	void createCategory(const SettingsElements::Category &category);
	void createSection(const SettingsElements::Section &section, QTabWidget *tabWidget);
	void createGroup(const SettingsElements::Group &group, QWidget *contentWidget, QFormLayout *layout);
	void createEntry(const SettingsElements::Entry &entry, QWidget *sectionWidget, QFormLayout *layout);

	void saveValues();
	void restoreValues();

	int calcSpacing(Qt::Orientation orientation);
	void updateWidth(int width);
	void resetListSize();

	QIcon loadIcon(const QUrl &icon);

	void searchInDialog(const QRegularExpression &regex);
	bool searchInCategory(const QRegularExpression &regex, QTabWidget *tab);
	bool searchInSection(const QRegularExpression &regex, QWidget *contentWidget);
	bool searchInGroup(const QRegularExpression &regex, QGroupBox *groupWidget);
	bool searchInEntry(const QRegularExpression &regex, QLabel *label, QWidget *content);

	QVariant readValue(const SettingsElements::Entry &entry) const;

public Q_SLOTS:
	void createUi();
	void entryChanged(const QString &key);

	void propertyChanged();
	void buttonBoxClicked(QAbstractButton *button);
	void filterTextChanged(const QString &searchText);
};

}

#endif // QTMVVM_SETTINGSDIALOG_P_H

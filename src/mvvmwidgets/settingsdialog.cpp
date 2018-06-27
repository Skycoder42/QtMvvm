#include "settingsdialog.h"
#include "settingsdialog_p.h"
#include "ui_settingsdialog.h"
#include "widgetspresenter_p.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QRegularExpression>

#include <QtMvvmCore/CoreApp>

#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>

#include <QtMvvmCore/CoreApp>
#include <QtMvvmCore/private/qtmvvm_logging_p.h>

using namespace QtMvvm;

SettingsDialog::SettingsDialog(ViewModel *viewModel, QWidget *parent) :
	QDialog{parent},
	d{new SettingsDialogPrivate{this, viewModel}}
{
	d->ui->setupUi(this);
	connect(d->ui->buttonBox, &QDialogButtonBox::clicked,
			d, &SettingsDialogPrivate::buttonBoxClicked);
	connect(d->ui->filterLineEdit, &QLineEdit::textChanged,
			d, &SettingsDialogPrivate::filterTextChanged);
	connect(d->viewModel, &SettingsViewModel::valueChanged,
			d, &SettingsDialogPrivate::entryChanged,
			Qt::QueuedConnection); // to detach updated from the bulk save operation
	connect(d->viewModel, &SettingsViewModel::resetAccepted,
			this, &SettingsDialog::accept);

	if(parentWidget()) {
		setWindowModality(Qt::WindowModal);
		setWindowFlags(Qt::Sheet | Qt::WindowCloseButtonHint | Qt::WindowContextHelpButtonHint);
	} else {
		setWindowModality(Qt::ApplicationModal);
		setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::WindowContextHelpButtonHint);
	}

#ifdef Q_OS_OSX
	auto font = d->ui->titleLabel->font();
	font.setPointSize(16);
	d->ui->titleLabel->setFont(font);
#endif

	int listSpacing = d->calcSpacing(Qt::Vertical);
	d->delegate = new CategoryItemDelegate(std::bind(&SettingsDialogPrivate::updateWidth, d, std::placeholders::_1),
										   d->ui->categoryListWidget->iconSize(),
										   qMax(qRound(listSpacing * (2./3.)), 1),
										   this);
	d->ui->categoryListWidget->setItemDelegate(d->delegate);
	d->ui->categoryListWidget->setSpacing(qMax(qRound(listSpacing / 3.), 1) - 1);

	int spacing = d->calcSpacing(Qt::Horizontal);
	d->ui->contentLayout->setSpacing(spacing);
	d->ui->categoryLineSpacer->changeSize(spacing,
										  0,
										  QSizePolicy::Fixed,
										  QSizePolicy::Fixed);

	connect(d->viewModel, &SettingsViewModel::beginLoadSetup,
			d, &SettingsDialogPrivate::createUi);
}

SettingsDialog::~SettingsDialog() = default;

QString SettingsDialog::labelFilterStyleSheet() const
{
	return QStringLiteral("QLabel {"
						  "    background-color: rgba(19,232,51,0.4);"
						  "    border: 1px solid rgba(19,196,45,0.8);"
						  "    border-radius: 4px;"
						  "}");
}

QUrl SettingsDialog::iconOverwrite() const
{
	return QStringLiteral("qrc:/de/skycoder42/qtmvvm/icons/settings.ico");
}

// ------------- Private Implementation -------------

const QString SettingsDialogPrivate::TabContentId(QStringLiteral("__qtmvvm_settings_widgets_tab_content"));

SettingsDialogPrivate::SettingsDialogPrivate(SettingsDialog *q_ptr, ViewModel *viewModel) :
	QObject(q_ptr),
	q(q_ptr),
	viewModel(static_cast<SettingsViewModel*>(viewModel)),
	ui(new Ui::SettingsDialog())
{}

SettingsDialogPrivate::~SettingsDialogPrivate() = default;

void SettingsDialogPrivate::createUi()
{
	auto icoUrl = q->iconOverwrite();
	if(icoUrl.isValid())
		viewModel->settingsSetupLoader()->changeDefaultIcon(icoUrl);
	setup = viewModel->loadSetup(QStringLiteral("widgets"));

	ui->filterLineEdit->setVisible(setup.allowSearch);
	ui->buttonBox->button(QDialogButtonBox::RestoreDefaults)->setVisible(setup.allowRestore && viewModel->canRestoreDefaults());

	for(const auto &category : qAsConst(setup.categories))
		createCategory(category);

	resetListSize();
	ui->categoryListWidget->setCurrentRow(0);
}

void SettingsDialogPrivate::entryChanged(const QString &key)
{
	auto content = keyMap.value(key);
	if(!content)
		return;
	auto info = entryMap.value(content);
	info.second.write(content, readValue(info.first));
}

void SettingsDialogPrivate::createCategory(const SettingsElements::Category &category)
{
	auto item = new QListWidgetItem();
	item->setText(category.title);
	item->setIcon(loadIcon(category.icon));
	item->setToolTip(category.tooltip.isNull() ? category.title : category.tooltip);
	item->setWhatsThis(item->toolTip());
	auto tab = new QTabWidget(ui->contentStackWidget);
	tab->setTabBarAutoHide(true);

	ui->contentStackWidget->addWidget(tab);
	ui->categoryListWidget->addItem(item);

	for(const auto &section : category.sections)
		createSection(section, tab);
}

void SettingsDialogPrivate::createSection(const SettingsElements::Section &section, QTabWidget *tabWidget)
{
	auto scrollArea = new QScrollArea(tabWidget);
	scrollArea->setWidgetResizable(true);
	scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setAutoFillBackground(true);
	auto pal = scrollArea->palette();
	pal.setColor(QPalette::Window, tabWidget->palette().color(QPalette::Base));
	scrollArea->setPalette(pal);
	scrollArea->setFrameShape(QFrame::NoFrame);

	auto scrollContent = new QWidget(scrollArea);
	scrollContent->setObjectName(TabContentId);
	auto layout = new QFormLayout(scrollContent);
	scrollContent->setLayout(layout);
	scrollArea->setWidget(scrollContent);

	auto index = tabWidget->addTab(scrollArea, loadIcon(section.icon), section.title);
	auto tooltip = section.tooltip.isNull() ? section.title : section.tooltip;
	tabWidget->tabBar()->setTabToolTip(index, tooltip);
	tabWidget->tabBar()->setTabWhatsThis(index, tooltip);

	for(const auto &group : section.groups)
		createGroup(group, scrollContent, layout);
}

void SettingsDialogPrivate::createGroup(const SettingsElements::Group &group, QWidget *contentWidget, QFormLayout *layout)
{
	QWidget *sectionWidget = nullptr;
	QFormLayout *sectionLayout = nullptr;
	if(group.title.isNull()) {
		sectionWidget = contentWidget;
		sectionLayout = layout;
	} else {
		auto groupBox = new QGroupBox(group.title, contentWidget);
		groupBox->setToolTip(group.tooltip.isNull() ? group.title : group.tooltip);
		layout->addRow(groupBox);
		sectionWidget = groupBox;
		sectionLayout = new QFormLayout(groupBox);
		groupBox->setLayout(sectionLayout);
	}

	for(const auto &entry : group.entries)
		createEntry(entry, sectionWidget, sectionLayout);
}

void SettingsDialogPrivate::createEntry(const SettingsElements::Entry &entry, QWidget *sectionWidget, QFormLayout *layout)
{
	QWidget *content = nullptr;
	if(entry.type == "action") {
		auto btn = new QPushButton(sectionWidget);
		for(auto it = entry.properties.constBegin(); it != entry.properties.constEnd(); it++)
			btn->setProperty(qUtf8Printable(it.key()), it.value());
		auto key = entry.key;
		auto args = entry.properties.value(QStringLiteral("args")).toMap();
		connect(btn, &QPushButton::clicked, this, [this, key, args](){
			viewModel->callAction(key, args);
		});
		content = btn;
	} else {
		try {
			auto widgetFactory = WidgetsPresenterPrivate::currentPresenter()->inputWidgetFactory();
			content = widgetFactory->createInput(entry.type, sectionWidget, entry.properties);
			auto property = content->metaObject()->userProperty();
			property.write(content, readValue(entry));
			if(property.hasNotifySignal()) {
				auto changedSlot = metaObject()->method(metaObject()->indexOfSlot("propertyChanged()"));
				connect(content, property.notifySignal(),
						this, changedSlot);
			} else
				changedEntries.insert(content);

			entryMap.insert(content, {entry, property});
			keyMap.insert(entry.key, content);
		} catch (PresenterException &e) {
			logWarning() << "Failed to create settings widget for key"
						 << entry.key
						 << "with error:" << e.what();
			content = new QLabel(tr("<i>Failed to load edit view!<i>"), sectionWidget);
			auto pal = content->palette();
			pal.setColor(QPalette::WindowText, Qt::darkRed);
			content->setPalette(pal);
		}
	}

	auto label = new QLabel(entry.title + tr(":"), sectionWidget);
	label->setBuddy(content);
	label->setToolTip(entry.tooltip.isNull() ? entry.title : entry.tooltip);
	label->setWhatsThis(label->toolTip());
	if(content->toolTip().isNull())
		content->setToolTip(label->toolTip());
	if(content->whatsThis().isNull())
		content->setWhatsThis(label->toolTip());

	layout->addRow(label, content);
}

void SettingsDialogPrivate::saveValues()
{
	for(auto it = changedEntries.begin(); it != changedEntries.end();) {
		auto widget = *it;
		auto info = entryMap.value(widget);
		viewModel->saveValue(info.first.key, info.second.read(widget));
		if(info.second.hasNotifySignal())
			it = changedEntries.erase(it);
		else
			it++;
	}
}

void SettingsDialogPrivate::restoreValues()
{
	if(!viewModel->canRestoreDefaults())
		return;
	viewModel->resetAll(setup);
}

int SettingsDialogPrivate::calcSpacing(Qt::Orientation orientation)
{
	auto baseSize = q->style()->pixelMetric(orientation == Qt::Horizontal ?
												QStyle::PM_LayoutHorizontalSpacing :
												QStyle::PM_LayoutVerticalSpacing);
	if(baseSize < 0)
		baseSize = q->style()->layoutSpacing(QSizePolicy::DefaultType, QSizePolicy::DefaultType, orientation);
	if(baseSize < 0) {
#ifdef Q_OS_OSX
		baseSize = 10;
#else
		baseSize = 6;
#endif
	}

	return baseSize;
}

void SettingsDialogPrivate::updateWidth(int width)
{
	if(width > maxWidthBase) {
		maxWidthBase = width;
		QStyle *style = ui->categoryListWidget->style();
		width += style->pixelMetric(QStyle::PM_ScrollBarExtent);
		width += style->pixelMetric(QStyle::PM_LayoutHorizontalSpacing);
		ui->categoryListWidget->setFixedWidth(width);
	}
}
void SettingsDialogPrivate::resetListSize()
{
	int max = ui->categoryListWidget->count();
	if(max <= 1) {
		ui->categoryContentWidget->hide();
		q->resize(q->width() - ui->categoryContentWidget->sizeHint().width(), q->height());
	} else {
		auto width = ui->categoryListWidget->sizeHint().width();
		ui->categoryListWidget->setFixedWidth(width);
		maxWidthBase = width;
	}
}

QIcon SettingsDialogPrivate::loadIcon(const QUrl &icon)
{
	if(icon.scheme() == QStringLiteral("qrc"))
		return QIcon(QLatin1Char(':') + icon.path());
	else
		return QIcon(icon.toLocalFile());
}

void SettingsDialogPrivate::searchInDialog(const QRegularExpression &regex)
{
	for(int i = 0, max = ui->categoryListWidget->count(); i < max; ++i) {
		auto item = ui->categoryListWidget->item(i);
		auto tab = qobject_cast<QTabWidget*>(ui->contentStackWidget->widget(i));
		if(!tab)
			continue;

		if(searchInCategory(regex, tab) ||
		   regex.match(item->text()).hasMatch()) {
			item->setHidden(false);
			if(ui->categoryListWidget->currentRow() == -1)
				ui->categoryListWidget->setCurrentRow(i);
		} else {
			item->setHidden(true);
			if(ui->categoryListWidget->currentRow() == i) {
				auto found = false;
				for(int j = 0; j < max; j++) {
					if(!ui->categoryListWidget->item(j)->isHidden()){
						ui->categoryListWidget->setCurrentRow(j);
						found = true;
						break;
					}
				}
				if(!found) {
					ui->categoryListWidget->setCurrentRow(-1);
					ui->contentStackWidget->setCurrentIndex(max);
				}
			}
		}
	}
}

bool SettingsDialogPrivate::searchInCategory(const QRegularExpression &regex, QTabWidget *tab)
{
	auto someFound = false;
	for(int i = 0, max = tab->count(); i < max; ++i) {
		if(searchInSection(regex, tab->widget(i)->findChild<QWidget*>(TabContentId)) ||
		   regex.match(tab->tabText(i)).hasMatch()){
			tab->setTabEnabled(i, true);
			someFound = true;
		} else
			tab->setTabEnabled(i, false);
	}

	return someFound;
}

bool SettingsDialogPrivate::searchInSection(const QRegularExpression &regex, QWidget *contentWidget)
{
	auto layout = qobject_cast<QFormLayout*>(contentWidget->layout());
	if(!layout)
		return false;

	auto someFound = false;
	for(int i = 0, max = layout->rowCount(); i < max; ++i) {
		auto spanItem = layout->itemAt(i, QFormLayout::SpanningRole);
		if(spanItem) {
			auto group = qobject_cast<QGroupBox*>(spanItem->widget());
			if(!group)
				continue;
			someFound |= searchInGroup(regex, group) ||
						 regex.match(group->title()).hasMatch();
		} else {
			auto label = qobject_cast<QLabel*>(layout->itemAt(i, QFormLayout::LabelRole)->widget());
			if(!label)
				continue;
			auto content = layout->itemAt(i, QFormLayout::FieldRole)->widget();
			someFound |= searchInEntry(regex, label, content);
		}
	}

	return someFound;
}

bool SettingsDialogPrivate::searchInGroup(const QRegularExpression &regex, QGroupBox *groupWidget)
{
	auto layout = qobject_cast<QFormLayout*>(groupWidget->layout());
	if(!layout)
		return false;

	auto someFound = false;
	for(int i = 0, max = layout->rowCount(); i < max; ++i) {
		auto label = qobject_cast<QLabel*>(layout->itemAt(i, QFormLayout::LabelRole)->widget());
		if(!label)
			continue;

		auto content = layout->itemAt(i, QFormLayout::FieldRole)->widget();
		someFound |= searchInEntry(regex, label, content);
	}

	return someFound;
}

bool SettingsDialogPrivate::searchInEntry(const QRegularExpression &regex, QLabel *label, QWidget *content)
{
	if(regex.pattern().isEmpty()) {
		label->setStyleSheet(QString());
		return false;
	}

	auto keys = entryMap.value(content).first.searchKeys;
	keys.append(label->text());
	for(const auto &key : keys) {
		if(regex.match(key).hasMatch()) {
			label->setStyleSheet(q->labelFilterStyleSheet());
			return true;
		}
	}

	label->setStyleSheet(QString());
	return false;
}

QVariant SettingsDialogPrivate::readValue(const SettingsElements::Entry &entry) const
{
	return CoreApp::safeCastInputType(entry.type, viewModel->loadValue(entry.key, entry.defaultValue));
}

void SettingsDialogPrivate::propertyChanged()
{
	auto widget = qobject_cast<QWidget*>(sender());
	if(widget)
		changedEntries.insert(widget);
}

void SettingsDialogPrivate::buttonBoxClicked(QAbstractButton *button)
{
	switch(ui->buttonBox->standardButton(button)) {
	case QDialogButtonBox::Ok:
		saveValues();
		q->accept();
		break;
	case QDialogButtonBox::Cancel:
		q->reject();
		break;
	case QDialogButtonBox::Apply:
		saveValues();
		break;
	case QDialogButtonBox::RestoreDefaults:
		restoreValues();
		break;
	default:
		Q_UNREACHABLE();
	}
}

void SettingsDialogPrivate::filterTextChanged(const QString &searchText)
{
	searchInDialog(QRegularExpression {
					   searchText,
					   QRegularExpression::CaseInsensitiveOption |
					   QRegularExpression::DontCaptureOption |
					   QRegularExpression::UseUnicodePropertiesOption
				   });
}



CategoryItemDelegate::CategoryItemDelegate(std::function<void(int)> updateFunc, QSize iconSize, int layoutSpacing, QObject *parent) :
	QStyledItemDelegate(parent),
	_iconSize(iconSize + QSize(0, layoutSpacing)),
	_updateFunc(std::move(updateFunc))
{}

QSize CategoryItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QSize size = QStyledItemDelegate::sizeHint(option, index);
	_updateFunc(size.width());
	return size.expandedTo(_iconSize);
}

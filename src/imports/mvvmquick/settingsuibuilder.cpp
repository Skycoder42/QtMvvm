#include "settingsuibuilder.h"

#include <QtCore/QRegularExpression>
#include <QtCore/QCoreApplication>

#include <QtMvvmCore/CoreApp>
#include <QtMvvmCore/Messages>

#include <QtQml/QQmlInfo>

#include <QtMvvmQuick/private/quickpresenter_p.h>

using namespace QtMvvm;

SettingsUiBuilder::SettingsUiBuilder(QObject *parent) :
	QObject(parent),
	_sectionFilterModel(new MultiFilterProxyModel(this)),
	_sectionModel(new SettingsSectionModel(this)),
	_entryFilterModel(new MultiFilterProxyModel(this)),
	_entryModel(new SettingsEntryModel(this))
{
	_sectionFilterModel->setSourceModel(_sectionModel);
	_sectionFilterModel->addFilterRoles(SettingsSectionModel::FilterRoles);
	_entryFilterModel->setSourceModel(_entryModel);
	_entryFilterModel->addFilterRoles(SettingsEntryModel::FilterRoles);

	connect(this, &SettingsUiBuilder::buildViewChanged,
			this, &SettingsUiBuilder::startBuildUi);
	connect(this, &SettingsUiBuilder::viewModelChanged,
			this, &SettingsUiBuilder::startBuildUi);
}

QString SettingsUiBuilder::filterText() const
{
	return _filterText;
}

void SettingsUiBuilder::loadSection(const SettingsElements::Section &section)
{
	auto inputFactory = QuickPresenterPrivate::currentPresenter()->inputViewFactory();
	_entryModel->setup(section, _viewModel, inputFactory);
#ifndef QT_NO_DEBUG
	qmlDebug(this) << "Loaded section: " << section.title;
#endif
	emit presentSection(_entryFilterModel);
}

void SettingsUiBuilder::showDialog(const QString &key, const QString &title, const QString &type, const QVariant &defaultValue, const QVariantMap &properties)
{
	if(type == QStringLiteral("action"))
		_viewModel->callAction(key, properties.value(QStringLiteral("args")).toMap());
	else {
#ifndef QT_NO_DEBUG
		qmlDebug(this) << "Creating input dialog for settings entry: " << key;
#endif
		getInput(title + tr(":"), QString(), qUtf8Printable(type), this, [this, key](const QVariant &value) {
			if(value.isValid())
				_viewModel->saveValue(key, value);
		}, _viewModel->loadValue(key, defaultValue), properties);
	}
}

void SettingsUiBuilder::restoreDefaults()
{
	if(!_viewModel->canRestoreDefaults())
		return;
	_viewModel->resetAll(_currentSetup);
}

void SettingsUiBuilder::setFilterText(QString filterText)
{
	if (_filterText == filterText)
		return;

	_filterText = std::move(filterText);
	emit filterTextChanged(_filterText);

	QRegularExpression regex(_filterText,
							 QRegularExpression::CaseInsensitiveOption |
							 QRegularExpression::UseUnicodePropertiesOption |
							 QRegularExpression::DontCaptureOption);
	_sectionFilterModel->setFilter(regex);
	_entryFilterModel->setFilter(regex);
}

void SettingsUiBuilder::startBuildUi()
{
	if(!_buildView || !_viewModel)
		return;

	_currentSetup = _viewModel->loadSetup(QStringLiteral("quick"));

	//search/restore properties
	_allowSearch = _currentSetup.allowSearch;
	emit allowSearchChanged(_allowSearch);
	_allowRestore = _currentSetup.allowRestore;
	emit allowRestoreChanged(_allowRestore);

	if(_currentSetup.categories.size() == 1 &&
	   _currentSetup.categories.first().sections.size() == 1)
		loadSection(_currentSetup.categories.first().sections.first());
	else {
		_sectionModel->setup(_currentSetup);
		emit presentOverview(_sectionFilterModel, _sectionModel->hasSections());
	}
}

void SettingsUiBuilder::setViewModel(SettingsViewModel *viewModel)
{
	if(_viewModel == viewModel)
		return;

	disconnect(_viewModel, &SettingsViewModel::resetAccepted,
			   this, &SettingsUiBuilder::closeSettings);
	_viewModel = viewModel;
	connect(_viewModel, &SettingsViewModel::resetAccepted,
			this, &SettingsUiBuilder::closeSettings);
	emit viewModelChanged(_viewModel);
}

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
	_buildView(nullptr),
	_viewModel(nullptr),
	_filterText(),
	_allowSearch(true),
	_allowRestore(true),
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

QUrl SettingsUiBuilder::svgEscape(QUrl url)
{
	if(url.scheme() == QStringLiteral("qrc")) {
		auto path = url.path();
		if(path.endsWith(QStringLiteral(".svg"))) {
			path.chop(4);
			path.prepend(QStringLiteral("image://svg"));
			return path;
		}
	}

	return url;
}

void SettingsUiBuilder::loadSection(const SettingsElements::Section &section)
{
	auto inputFactory = QuickPresenterPrivate::currentPresenter()->inputViewFactory();
	_entryModel->setup(section, _viewModel, inputFactory);
	emit presentSection(_entryFilterModel);
}

void SettingsUiBuilder::showDialog(const QString &key, const QString &title, const QString &type, const QVariantMap &properties)
{
	if(type == QStringLiteral("action"))
		_viewModel->callAction(key, properties.value(QStringLiteral("args")).toMap());
	else {
		getInput(title + tr(":"), QString(), qUtf8Printable(type), this, [this, key](QVariant value) {
			if(value.isValid())
				_viewModel->saveValue(key, value);
		}, _viewModel->loadValue(key), properties);
	}
}

void SettingsUiBuilder::restoreDefaults()
{
	if(!_viewModel->canRestoreDefaults())
		return;

	auto result = CoreApp::showDialog(_viewModel->restoreConfig());
	connect(result, &MessageResult::dialogDone, this, [this](MessageConfig::StandardButton btn) {
		if(btn != MessageConfig::Yes)
			return;
		emit closeSettings();
	}, Qt::QueuedConnection);
}

void SettingsUiBuilder::setFilterText(QString filterText)
{
	if (_filterText == filterText)
		return;

	_filterText = filterText;
	emit filterTextChanged(filterText);

	QRegularExpression regex(filterText,
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

	auto setup = _viewModel->loadSetup(QStringLiteral("quick"));

	//search/restore properties
	_allowSearch = setup.allowSearch;
	emit allowSearchChanged(_allowSearch);
	_allowRestore = setup.allowRestore;
	emit allowRestoreChanged(_allowRestore);

	if(setup.categories.size() == 1 &&
	   setup.categories.first().sections.size() == 1)
		loadSection(setup.categories.first().sections.first());
	else {
		_sectionModel->setup(setup);
		emit presentOverview(_sectionFilterModel, _sectionModel->hasSections());
	}
}

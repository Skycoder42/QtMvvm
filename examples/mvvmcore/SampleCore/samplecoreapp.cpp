#include "samplecoreapp.h"
#include "sampleviewmodel.h"
#include "drawerviewmodel.h"
#include "tabviewmodel.h"

#include <QtCore/QLoggingCategory>
#include <QtCore/QCommandLineParser>
#include <QtMvvmCore/QtMvvmCoreVersion>

SampleCoreApp::SampleCoreApp(QObject *parent) :
	CoreApp(parent),
	_showDrawer(false)
{
	QCoreApplication::setApplicationName(QStringLiteral("QtMvvmSample"));
	QCoreApplication::setApplicationVersion(QStringLiteral(QTMVVMCORE_VERSION_STR));
	QCoreApplication::setOrganizationName(QStringLiteral("Skycoder42"));
}

bool SampleCoreApp::showDrawer() const
{
	return _showDrawer;
}

void SampleCoreApp::setShowDrawer(bool showDrawer)
{
	if (_showDrawer == showDrawer)
		return;

	_showDrawer = showDrawer;
	emit showDrawerChanged(_showDrawer);
}

void SampleCoreApp::performRegistrations()
{
	Q_INIT_RESOURCE(sample_core);

	qRegisterMetaType<TabViewModel*>();
	QtMvvm::registerInterfaceConverter<IEventService>();

	QLoggingCategory::setFilterRules(QStringLiteral("qtmvvm.debug=true"));
}

int SampleCoreApp::startApp(const QStringList &arguments)
{
	QCommandLineParser parser;
	parser.addVersionOption();
	parser.addHelpOption();

	parser.addOption({
						 {QStringLiteral("a"), QStringLiteral("active")},
						 QStringLiteral("Start with the active checkbox already checked")
					 });
	parser.addPositionalArgument(QStringLiteral("names"),
								 QStringLiteral("A list of names, joined together as one string"),
								 QStringLiteral("[names...]"));

	if(!autoParse(parser, arguments))
		return EXIT_SUCCESS;

	QVariantHash args;
	args.insert(SampleViewModel::KeyActive, parser.isSet(QStringLiteral("active")));
	args.insert(SampleViewModel::KeyNames, parser.positionalArguments());
	show<SampleViewModel>(args);
	if(_showDrawer)
		show<DrawerViewModel>();
	return EXIT_SUCCESS;
}

#include "samplecoreapp.h"
#include "sampleviewmodel.h"

#include <QtCore/QCommandLineParser>
#include <QtMvvmCore/QtMvvmCoreVersion>

SampleCoreApp::SampleCoreApp(QObject *parent) :
	CoreApp(parent)
{
	QCoreApplication::setApplicationVersion(QStringLiteral(QTMVVMCORE_VERSION_STR));
	QCoreApplication::setOrganizationName(QStringLiteral("Skycoder42"));
}

void SampleCoreApp::performRegistrations()
{
	Q_INIT_RESOURCE(sample_core);

	QtMvvm::registerInterfaceConverter<IEventService>();
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
								 QStringLiteral("[name..."));

	if(!autoParse(parser, arguments))
		return EXIT_SUCCESS;

	QVariantHash args;
	args.insert(SampleViewModel::KeyActive, parser.isSet(QStringLiteral("active")));
	args.insert(SampleViewModel::KeyNames, parser.positionalArguments());
	show<SampleViewModel>(args);
	return EXIT_SUCCESS;
}

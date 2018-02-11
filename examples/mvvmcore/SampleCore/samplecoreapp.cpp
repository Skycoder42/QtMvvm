#include "samplecoreapp.h"
#include "sampleviewmodel.h"

#include <QtCore/QCommandLineParser>

SampleCoreApp::SampleCoreApp(QObject *parent) :
	CoreApp(parent)
{}

void SampleCoreApp::performRegistrations()
{
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

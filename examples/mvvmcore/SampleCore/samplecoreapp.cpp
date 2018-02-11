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
						 {QStringLiteral("n"), QStringLiteral("nothing")},
						 QStringLiteral("Pointless action")
					 });

	if(!autoParse(parser, arguments))
		return EXIT_SUCCESS;

	show<SampleViewModel>();
	return EXIT_SUCCESS;
}

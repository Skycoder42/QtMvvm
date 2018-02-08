#include "samplecoreapp.h"

#include <QtCore/QCommandLineParser>

QTMVVM_REGISTER_CORE_APP(SampleCoreApp)

SampleCoreApp::SampleCoreApp(QObject *parent) :
	CoreApp(parent)
{}

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

	//TODO implement app code
	return EXIT_FAILURE;
}

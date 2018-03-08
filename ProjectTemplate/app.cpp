#include "%{AppHdrName}"
#include "%{VmHdrName}"

#include <QtCore/QCommandLineParser>

%{AppCn}::%{AppCn}(QObject *parent) :
	CoreApp(parent)
{
	QCoreApplication::setApplicationName(QStringLiteral("%{ProjectName}"));
	QCoreApplication::setApplicationVersion(QStringLiteral("1.0.0"));
	QCoreApplication::setOrganizationName(QStringLiteral("Example Organization"));
}

void %{AppCn}::performRegistrations()
{
	//if you are using a qt resource (e.g. "%{AppQrcFile}"), initialize it here
@if '%{UseSettings}'
	Q_INIT_RESOURCE(%{AppQrcName});
@else
	//Q_INIT_RESOURCE(%{AppQrcName});
@endif
}

int %{AppCn}::startApp(const QStringList &arguments)
{
	QCommandLineParser parser;
	parser.addVersionOption();
	parser.addHelpOption();
	
	//add more options
	
	//shows help or version automatically
	if(!autoParse(parser, arguments))
		return EXIT_SUCCESS;
	
	//show a viewmodel to complete the startup
	show<%{VmCn}>();
	return EXIT_SUCCESS;
}

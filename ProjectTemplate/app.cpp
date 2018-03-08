#include "%{AppHdrName}"

%{AppCn}::%{AppCn}(QObject *parent) :
	CoreApp(parent),
	mainControl(nullptr)
{
	//register metatypes etc here, just like you would do in your main before call QCoreApplication::exec

	//if you are using a qt resource (e.g. "%{AppQrcFile}"), initialize it here
@if '%{UseSettings}'
	Q_INIT_RESOURCE(%{AppQrcName});
@else
	//Q_INIT_RESOURCE(%{AppQrcName});
@endif
}

void %{AppCn}::setupParser(QCommandLineParser &parser, bool &allowInvalid) const
{
	CoreApp::setupParser(parser, allowInvalid);
	//add additional command line arguments etc here
}

bool %{AppCn}::startApp(const QCommandLineParser &parser)
{
	//shows help or version automatically
	if(autoShowHelpOrVersion(parser))
		return true;

	//use this method to create services, controls, etc

	//create and show the inital control
	mainControl = new %{ControlCn}(this);
	showControl(mainControl);

	return true;
}

void %{AppCn}::aboutToQuit()
{
	//if you need to perform any cleanups, do it here
}

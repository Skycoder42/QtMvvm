#include <QApplication>
#include <widgetpresenter.h>
#include <%{AppHdrName}>

@if '%{UseSettings}'
#include <settingsdialog.h>
@endif
#include "%{WindowHdrName}"

//register the core app to be used
REGISTER_CORE_APP(%{AppCn})

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	// automatically sets "WidgetPresenter" as presenter and registers the %{WindowName} class as a widget
	// the control this widget belongs to is detected automatically via naming conventions
	WidgetPresenter::registerWidget<%{WindowCn}>();
@if '%{UseSettings}'
	WidgetPresenter::registerWidget<SettingsDialog>();
@endif

	return a.exec();
}

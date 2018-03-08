#include "%{ControlHdrName}"
@if '%{UseSettings}'
#include <settingscontrol.h>
@endif

%{ControlCn}::%{ControlCn}(QObject *parent) :
	Control(parent),
	_text(QStringLiteral("hello world"))
{}

QString %{ControlCn}::text() const
{
	return _text;
}

@if '%{UseSettings}'
void %{ControlCn}::showSettings()
{
	auto settings = new SettingsControl(this);
	settings->setDeleteOnClose(true);
	settings->show();
}

@endif
void %{ControlCn}::setText(QString text)
{
	if (_text == text)
		return;
	
	_text = text;
	emit textChanged(_text);
}

void %{ControlCn}::onShow()
{
	qDebug("%{ControlName} gui is now visible");
	//logic to execute when the gui is shown
}

void %{ControlCn}::onClose()
{
	qDebug("%{ControlName} gui is now closed");
	//logic to execute when the gui was closed
}

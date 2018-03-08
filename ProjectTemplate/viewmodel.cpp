#include "%{VmHdrName}"
@if '%{UseSettings}'

#include <QtMvvmCore/SettingsViewModel>
@endif

%{VmCn}::%{VmCn}(QObject *parent) :
	ViewModel(parent),
	_text(QStringLiteral("hello world"))
{}

QString %{VmCn}::text() const
{
	return _text;
}

@if '%{UseSettings}'
void %{VmCn}::showSettings()
{
	show<QtMvvm::SettingsViewModel>();
}

@endif
void %{VmCn}::setText(const QString &text)
{
	if (_text == text)
		return;
	
	_text = text;
	emit textChanged(_text);
}

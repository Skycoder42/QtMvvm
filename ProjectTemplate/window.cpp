#include "%{WindowHdrName}"
#include "ui_%{WindowHdrName}"
#include <qtmvvmbinding.h>

%{WindowCn}::%{WindowCn}(Control *mControl, QWidget *parent) :
	QMainWindow(parent),
	control(static_cast<%{ControlCn}*>(mControl)),
	ui(new Ui::%{WindowCn})
{
	ui->setupUi(this);
@if '%{UseSettings}'

	connect(ui->actionSettings, &QAction::triggered,
			control, &%{ControlCn}::showSettings);
@endif

	QtMvvmBinding::bind(control, "text", ui->lineEdit, "text");
	QtMvvmBinding::bind(control, "text", ui->label, "text", QtMvvmBinding::OneWayFromControl);
}

%{WindowCn}::~%{WindowCn}()
{
	delete ui;
}

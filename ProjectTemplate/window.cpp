#include "%{WindowHdrName}"
#include "ui_%{WindowHdrName}"

#include <QtMvvmCore/Binding>

%{WindowCn}::%{WindowCn}(QtMvvm::ViewModel *viewModel, QWidget *parent) :
	QMainWindow(parent),
	_viewModel(static_cast<%{VmCn}*>(viewModel)),
	_ui(new Ui::%{WindowCn})
{
	_ui->setupUi(this);
@if '%{UseSettings}'

	connect(_ui->actionSettings, &QAction::triggered,
			_viewModel, &%{VmCn}::showSettings);
@endif

	QtMvvm::bind(_viewModel, "text",
				 _ui->lineEdit, "text");
	QtMvvm::bind(_viewModel, "text",
				 _ui->label, "text",
				 QtMvvm::Binding::OneWayToView);
}

%{WindowCn}::~%{WindowCn}()
{
	delete _ui;
}

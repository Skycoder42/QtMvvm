#include "qqmlmvvmbinding.h"
using namespace QtMvvm;

QQmlMvvmBinding::QQmlMvvmBinding(QObject *parent) :
	QObject(parent),
	QQmlParserStatus(),
	_binding(),
	_completed(false),
	_viewModel(nullptr),
	_viewModelProperty(),
	_view(parent),
	_viewProperty(),
	_type(TwoWay),
	_viewModelChangeSignal(),
	_viewChangeSignal()
{
	connect(this, &QQmlMvvmBinding::viewModelChanged,
			this, &QQmlMvvmBinding::resetBinding);
	connect(this, &QQmlMvvmBinding::viewModelPropertyChanged,
			this, &QQmlMvvmBinding::resetBinding);
	connect(this, &QQmlMvvmBinding::viewChanged,
			this, &QQmlMvvmBinding::resetBinding);
	connect(this, &QQmlMvvmBinding::viewPropertyChanged,
			this, &QQmlMvvmBinding::resetBinding);
	connect(this, &QQmlMvvmBinding::typeChanged,
			this, &QQmlMvvmBinding::resetBinding);
}

void QQmlMvvmBinding::classBegin() {}

void QQmlMvvmBinding::componentComplete()
{
	_completed = true;
	resetBinding();
}

QQmlMvvmBinding::BindingDirection QQmlMvvmBinding::type() const
{
	return _type;
}

bool QQmlMvvmBinding::isValid() const
{
	return _binding.isValid();
}

void QQmlMvvmBinding::setType(BindingDirection type)
{
	if (_type == type)
		return;

	_type = type;
	emit typeChanged(_type);
}

void QQmlMvvmBinding::unbind()
{
	_binding.unbind();
}

void QQmlMvvmBinding::resetBinding()
{
	if(!_completed || !_viewModel || !_view)
		return;

	_binding.unbind();
	_binding = QtMvvm::bind(_viewModel, qUtf8Printable(_viewModelProperty),
							_view, qUtf8Printable(_viewProperty),
							static_cast<Binding::BindingDirection>(static_cast<int>(_type)),
							_viewModelChangeSignal.isEmpty() ? nullptr : qUtf8Printable(_viewModelChangeSignal),
							_viewChangeSignal.isEmpty() ? nullptr : qUtf8Printable(_viewChangeSignal));
}

#include "qqmlmvvmbinding.h"
using namespace QtMvvm;
#include <QtQml>

QQmlMvvmBinding::QQmlMvvmBinding(QObject *parent) :
	QObject(parent),
	QQmlParserStatus()
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

	connect(this, &QQmlMvvmBinding::viewChanged,
			this, &QQmlMvvmBinding::viewWasSet);
	connect(this, &QQmlMvvmBinding::viewModelChanged,
			this, &QQmlMvvmBinding::viewModelWasSet);
}

void QQmlMvvmBinding::classBegin() {}

void QQmlMvvmBinding::componentComplete()
{
	// auto-detect parent item a view
	if(!_viewSet) {
		_view = parent();
		emit viewChanged(_view);
	}

	// auto-detect parent items viewmodel
	if(_view && !_viewModelSet) {
		auto vmo = _view->metaObject();
		auto vmPropIdx = vmo->indexOfProperty("viewModel");
		if(vmPropIdx != -1) {
			auto vm = vmo->property(vmPropIdx).read(_view).value<QObject*>();
			if(vm) {
				_viewModel = vm;
				emit viewModelChanged(_viewModel);
			}
		}
	}

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

void QQmlMvvmBinding::viewWasSet()
{
	_viewSet = true;
}

void QQmlMvvmBinding::viewModelWasSet()
{
	_viewModelSet = true;
}

#include "binding.h"
#include "binding_p.h"
#include "qtmvvm_logging_p.h"
using namespace QtMvvm;

Binding QtMvvm::bind(QObject *viewModel, const char *viewModelProperty, QObject *view, const char *viewProperty, Binding::BindingDirection type, const char *viewModelChangeSignal, const char *viewChangeSignal)
{
	if(!viewModel) {
		logWarning() << "ViewModel must not be null";
		return {};
	}
	auto moModel = viewModel->metaObject();
	auto piModel = moModel->indexOfProperty(viewModelProperty);
	if(piModel == -1) {
		logWarning() << "ViewModel of type" << moModel->className()
					 << "has no property named" << viewModelProperty;
		return {};
	}

	if(!view) {
		logWarning() << "View must not be null";
		return {};
	}
	auto moView = view->metaObject();
	auto piView = moView->indexOfProperty(viewProperty);
	if(piView == -1) {
		logWarning() << "View of type" << moView->className()
					 << "has no property named" << viewProperty;
		return {};
	}

	return bind(viewModel, moModel->property(piModel), view, moView->property(piView), type, viewModelChangeSignal, viewChangeSignal);
}

Binding QtMvvm::bind(QObject *viewModel, const QMetaProperty &viewModelProperty, QObject *view, const QMetaProperty &viewProperty, Binding::BindingDirection type, const char *viewModelChangeSignal, const char *viewChangeSignal)
{
	if(!viewModel) {
		qWarning() << "ViewModel must not be null";
		return {};
	}
	QMetaMethod smModel;
	if(viewModelChangeSignal) {
		auto moModel = viewModel->metaObject();
		auto siModel = moModel->indexOfSignal(viewModelChangeSignal);
		if(siModel == -1) {
			logWarning() << "ViewModel of type" << moModel->className()
						 << "has no signal named" << viewModelChangeSignal;
			return {};
		}
		smModel = moModel->method(siModel);
	}

	if(!view) {
		qWarning() << "View must not be null";
		return {};
	}
	QMetaMethod smView;
	if(viewChangeSignal) {
		auto moView = view->metaObject();
		auto siView = moView->indexOfSignal(viewChangeSignal);
		if(siView == -1) {
			logWarning() << "View of type" << moView->className()
						 << "has no signal named" << viewChangeSignal;
			return {};
		}
		smView = moView->method(siView);
	}

	return BindingPrivate::bind(viewModel, viewModelProperty, view, viewProperty, type, smModel, smView);
}

Binding QtMvvm::bind(QObject *viewModel, const QMetaProperty &viewModelProperty, QObject *view, const QMetaProperty &viewProperty, Binding::BindingDirection type, const QMetaMethod &viewModelChangeSignal, const QMetaMethod &viewChangeSignal)
{
	//check not null
	if(!viewModel) {
		qWarning() << "ViewModel must not be null";
		return {};
	}
	if(!view) {
		qWarning() << "View must not be null";
		return {};
	}

	return BindingPrivate::bind(viewModel, viewModelProperty, view, viewProperty, type, viewModelChangeSignal, viewChangeSignal);
}


Binding::Binding() :
	d(nullptr)
{}

Binding::Binding(QPointer<BindingPrivate> d_ptr) :
	d(std::move(d_ptr))
{}

Binding::~Binding() = default;

bool Binding::isValid() const
{
	return d;
}

Binding::operator bool() const
{
	return d;
}

void Binding::unbind()
{
	if(d) {
		if(d->modelToViewConnection)
			QObject::disconnect(d->modelToViewConnection);
		if(d->viewToModelConnection)
			QObject::disconnect(d->viewToModelConnection);
		d->deleteLater();
		d.clear();
	}
}

// ------------- Private Implementation -------------

Binding BindingPrivate::bind(QObject *viewModel, QMetaProperty viewModelProperty, QObject *view, QMetaProperty viewProperty, Binding::BindingDirection type, QMetaMethod viewModelChangeSignal, QMetaMethod viewChangeSignal)
{
	QPointer<BindingPrivate> binderPrivate = new BindingPrivate(viewModel, viewModelProperty, view, viewProperty);

	if(type.testFlag(Binding::SingleInit))
		binderPrivate->init();
	if(type.testFlag(Binding::OneWayToView))
		binderPrivate->bindFrom(viewModelChangeSignal);
	if(type.testFlag(Binding::OneWayToViewModel))
		binderPrivate->bindTo(viewChangeSignal);

	return binderPrivate;
}

BindingPrivate::BindingPrivate(QObject *viewModel, QMetaProperty viewModelProperty, QObject *view, QMetaProperty viewProperty) :
	QObject(view),
	viewModel(viewModel),
	view(view),
	viewModelProperty(viewModelProperty),
	viewProperty(viewProperty)
{
	connect(viewModel, &QObject::destroyed,
			this, &BindingPrivate::deleteLater);
}

void BindingPrivate::viewModelTrigger()
{
	viewProperty.write(view, viewModelProperty.read(viewModel));
}

void BindingPrivate::viewTrigger()
{
	viewModelProperty.write(viewModel, viewProperty.read(view));
}

void BindingPrivate::init()
{
	testReadable(viewModelProperty, false);
	testWritable(viewProperty, true);
	viewProperty.write(view, viewModelProperty.read(viewModel));
}

void BindingPrivate::bindFrom(QMetaMethod changeSignal)
{
	//testReadable and testWritable are alredy tested by the init() method
	if(!changeSignal.isValid()) {
		testNotifier(viewModelProperty, false);
		changeSignal = viewModelProperty.notifySignal();
	}
	auto trigger = metaObject()->method(metaObject()->indexOfSlot("viewModelTrigger()"));
	modelToViewConnection = connect(viewModel, changeSignal, this, trigger);
}

void BindingPrivate::bindTo(QMetaMethod changeSignal)
{
	testReadable(viewProperty, true);
	testWritable(viewModelProperty, false);
	if(!changeSignal.isValid()) {
		testNotifier(viewProperty, true);
		changeSignal = viewProperty.notifySignal();
	}
	auto trigger = metaObject()->method(metaObject()->indexOfSlot("viewTrigger()"));
	viewToModelConnection = connect(view, changeSignal, this, trigger);
}

void BindingPrivate::testReadable(const QMetaProperty &property, bool asView) const
{
	if(!property.isReadable()) {
		logWarning() << (asView ? "View" : "ViewModel")
					 << "property" << property.name()
					 << "of" << property.enclosingMetaObject()->className()
					 << "is not readable";
	}
}

void BindingPrivate::testWritable(const QMetaProperty &property, bool asView) const
{
	if(!property.isWritable()) {
		logWarning() << (asView ? "View" : "ViewModel")
					 << "property" << property.name()
					 << "of" << property.enclosingMetaObject()->className()
					 << "is not writable";
	}
}

void BindingPrivate::testNotifier(const QMetaProperty &property, bool asView) const
{
	if(!property.hasNotifySignal()) {
		logWarning() << (asView ? "View" : "ViewModel")
					 << "property" << property.name()
					 << "of" << property.enclosingMetaObject()->className()
					 << "has no notify singal";
	}
}

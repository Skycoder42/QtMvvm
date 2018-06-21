#include "qqmlviewplaceholder.h"
#include <QtCore/QMetaMethod>
#include <QtQml/QQmlInfo>
#include <QtQml/QQmlProperty>
#include <QtMvvmCore/CoreApp>
using namespace QtMvvm;

QQmlViewPlaceholder::QQmlViewPlaceholder(QQuickItem *parent) :
	QQuickItem{parent}
{
	// auto presenting
	connect(this, &QQmlViewPlaceholder::viewModelTypeChanged,
			this, &QQmlViewPlaceholder::presentIfReady);
	connect(this, &QQmlViewPlaceholder::autoPresentChanged,
			this, &QQmlViewPlaceholder::presentIfReady);

	// size changes
	connect(this, &QQmlViewPlaceholder::widthChanged,
			this, &QQmlViewPlaceholder::resizeView);
	connect(this, &QQmlViewPlaceholder::heightChanged,
			this, &QQmlViewPlaceholder::resizeView);
	connect(this, &QQmlViewPlaceholder::autoResizeViewChanged,
			this, &QQmlViewPlaceholder::resizeView);
}

bool QQmlViewPlaceholder::presentItem(const QVariant &item)
{
	// check if the parameter is valid
	auto quickItem = item.value<QQuickItem*>();
	if(!quickItem) {
		qmlWarning(this) << "presentItem called with invalid item of type " << item.typeName();
		return false;
	}

	// handle already existing view case
	if(_loadedView) {
		if(_replaceViews) { // quick discard without reenableing all children
			_loadedView->setVisible(false);
			_loadedView->deleteLater();
			_loadedView = nullptr;
		} else {
			qmlWarning(this) << R"(A view has already been presented. Discard it first via "discardView" or set "replaceViews" to true)";
			return false;
		}
	}

	// add
	_loadedView = quickItem;
	quickItem->setParent(this);
	quickItem->setParentItem(this);
	resizeView();
	quickItem->setVisible(true);

	// hide all children
	for(auto child : childItems()) {
		if(child != _loadedView)
			child->setVisible(false);
	}

	emit loadedViewChanged(_loadedView);
	return true;
}

bool QQmlViewPlaceholder::closeAction()
{
	if(!_loadedView)
		return false;

	// call close on the view
	auto vMetaObject = _loadedView->metaObject();
	auto cMethodIndex = vMetaObject->indexOfMethod("closeAction()");
	if(cMethodIndex != -1) {
		auto cMethod = vMetaObject->method(cMethodIndex);
		QVariant ok = false;
		cMethod.invoke(_loadedView, Q_RETURN_ARG(QVariant, ok));
		if(ok.toBool())
			return true;
	}

	// close the view itself (if wished)
	if(_closeViewOnAction) {
		discardView();
		return true;
	}

	// otherwise: nothing closed
	return false;
}

void QQmlViewPlaceholder::setParentViewModel(ViewModel *parentViewModel)
{
	// first: clear the auto-connected viewmodel, if required
	if(_clearParentVmCon && _parentVmCon)
		disconnect(_parentVmCon);

	// then: set property as usual
	if(_parentViewModel == parentViewModel)
		return;

	_parentViewModel = parentViewModel;
	emit parentViewModelChanged(_parentViewModel);

	// check the vm parent for a presenter method
	auto view = qobject_cast<QQuickItem*>(_parentViewModel->parent());
	if(view) {
		if(view->metaObject()->indexOfMethod("presentItem(QVariant)") == -1)
			qmlWarning(this) << R"(Parent item of "parentViewModel" does not have a "presentItem" method. Check the ViewPlaceholder documentation!)";
	} else
		qmlWarning(this) << R"(Parent item of "parentViewModel" is not an Item)";

	presentIfReady();
}

QQuickItem *QQmlViewPlaceholder::loadedView() const
{
	return _loadedView;
}

void QQmlViewPlaceholder::componentComplete()
{
	// auto-set the vm if not already set
	if(!_parentViewModel)
		getParentViewModel();

	// last step: call base implementation, then present
	QQuickItem::componentComplete();
	_isReady = true;
	presentIfReady();
}

void QQmlViewPlaceholder::presentView()
{
	CoreApp::show(qUtf8Printable(_viewModelType), _showParams, _parentViewModel);
}

void QQmlViewPlaceholder::discardView()
{
	// hide view
	_loadedView->setVisible(false);

	// show all children again
	for(auto child : childItems()) {
		if(child != _loadedView)
			child->setVisible(true);
	}

	// now delete it
	_loadedView->deleteLater();
	_loadedView = nullptr;
	emit loadedViewChanged(nullptr);
}

void QQmlViewPlaceholder::parentItemVmChanged(ViewModel *viewModel)
{
	// set vm without clearing the connection
	_clearParentVmCon = false;
	setParentViewModel(viewModel);
	_clearParentVmCon = true;
}

void QQmlViewPlaceholder::presentIfReady()
{
	if(_isReady &&
	   _autoPresent &&
	   !_loadedView &&
	   _parentViewModel &&
	   !_viewModelType.isEmpty())
		presentView();
}

void QQmlViewPlaceholder::resizeView()
{
	if(_loadedView && _autoResizeView) {
		_loadedView->setWidth(width());
		_loadedView->setHeight(height());
	}
}

void QQmlViewPlaceholder::getParentViewModel()
{
	auto pItem = parentItem();
	if(!pItem)
		return;

	QQmlProperty vmProp{pItem, QStringLiteral("viewModel")};
	if(!vmProp.isValid())
		return;

	// set the vm from the property
	auto vm = vmProp.read().value<ViewModel*>();
	if(vm)
		setParentViewModel(vm); // no warning here - might be ok for lazy presented vms

	// connect to further changes, via helper slot
	auto cSlotIndex = metaObject()->indexOfSlot("parentItemVmChanged(QtMvvm::ViewModel*)");
	Q_ASSERT(cSlotIndex != -1);
	if(_parentVmCon)
		disconnect(_parentVmCon);
	_parentVmCon = connect(pItem, vmProp.property().notifySignal(),
						   this, metaObject()->method(cSlotIndex));
}

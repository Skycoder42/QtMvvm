#include "qqmlviewplaceholder.h"
#include <QtCore/QMetaMethod>
#include <QtQml/QQmlInfo>
#include <QtQml/QQmlProperty>
#include <QtMvvmCore/CoreApp>
using namespace QtMvvm;

QQmlViewPlaceholder::QQmlViewPlaceholder(QQuickItem *parent) :
	QQuickItem{parent}
{
	setImplicitSize(0, 0); // init to 0

	// size changes
	connect(this, &QQmlViewPlaceholder::widthChanged,
			this, &QQmlViewPlaceholder::resizeView);
	connect(this, &QQmlViewPlaceholder::heightChanged,
			this, &QQmlViewPlaceholder::resizeView);
	connect(this, &QQmlViewPlaceholder::autoResizeViewChanged,
			this, &QQmlViewPlaceholder::resizeView);
}

bool QQmlViewPlaceholder::presentItem(QQuickItem *item)
{
	// handle already existing view case
	if(_loadedView) {
		if(_replaceViews) { // quick discard without reenableing all children
			disconnectSizeChanges(false);
			_loadedView->setVisible(false);
			_loadedView->deleteLater();
			_loadedView = nullptr;
		} else {
			qmlWarning(this) << R"(A view has already been presented. Discard it first via "discardView" or set "replaceViews" to true)";
			return false;
		}
	}

	// add
	_loadedView = item;
	_loadedView->setParent(this);
	_loadedView->setParentItem(this);
	connectSizeChanges();
	resizeView();
	_loadedView->setVisible(true);

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

QQuickItem *QQmlViewPlaceholder::loadedView() const
{
	return _loadedView;
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
	disconnectSizeChanges(true);
	_loadedView->deleteLater();
	_loadedView = nullptr;
	emit loadedViewChanged(nullptr);
}

void QQmlViewPlaceholder::resizeView()
{
	if(_loadedView && _autoResizeView) {
		_loadedView->setWidth(width());
		_loadedView->setHeight(height());
	}
}

void QQmlViewPlaceholder::updateImpHeight()
{
	setImplicitHeight(_loadedView->implicitHeight());
}

void QQmlViewPlaceholder::updateImpWidth()
{
	setImplicitWidth(_loadedView->implicitWidth());
}

void QQmlViewPlaceholder::connectSizeChanges()
{
	connect(_loadedView, &QQuickItem::implicitWidthChanged,
			this, &QQmlViewPlaceholder::updateImpWidth);
	connect(_loadedView, &QQuickItem::implicitHeightChanged,
			this, &QQmlViewPlaceholder::updateImpHeight);
	setImplicitSize(_loadedView->implicitWidth(),
					_loadedView->implicitHeight());
}

void QQmlViewPlaceholder::disconnectSizeChanges(bool resetSize)
{
	disconnect(_loadedView, &QQuickItem::implicitWidthChanged,
			   this, &QQmlViewPlaceholder::updateImpWidth);
	disconnect(_loadedView, &QQuickItem::implicitHeightChanged,
			   this, &QQmlViewPlaceholder::updateImpHeight);
	if(resetSize)
		setImplicitSize(0, 0);
}

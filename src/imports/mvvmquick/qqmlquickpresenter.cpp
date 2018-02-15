#include "qqmlquickpresenter.h"

#include <QtQml/QtQml>

#include <QtQuick/QQuickItem>

#include <QtMvvmQuick/private/quickpresenter_p.h>

using namespace QtMvvm;

QQmlQuickPresenter::QQmlQuickPresenter(QQmlEngine *engine) :
	QObject(engine),
	_engine(engine),
	_latestComponent(),
	_componentCache(),
	_loadCache()
{
	QuickPresenterPrivate::setQmlPresenter(this);
}

bool QQmlQuickPresenter::isViewLoading() const
{
	return _latestComponent;
}

qreal QQmlQuickPresenter::loadingProgress() const
{
	return _latestComponent ? _latestComponent->progress() : -1.0;
}

void QQmlQuickPresenter::present(ViewModel *viewModel, const QVariantHash &params, const QUrl &viewUrl, QPointer<ViewModel> parent)
{
	auto component = _componentCache.object(viewUrl);
	if(component)
		addObject(component, viewModel, params, parent);
	else {
		//create component (and replace latest)
		if(_latestComponent) {
			disconnect(_latestComponent, &QQmlComponent::progressChanged,
					   this, &QQmlQuickPresenter::loadingProgressChanged);
		}
		_latestComponent = new QQmlComponent(_engine, this);
		_loadCache.insert(_latestComponent, std::make_tuple(viewModel, params, parent));

		//setup ui status
		emit viewLoadingChanged(true);
		emit loadingProgressChanged(0.0);
		connect(_latestComponent, &QQmlComponent::progressChanged,
				this, &QQmlQuickPresenter::loadingProgressChanged);
		connect(_latestComponent, &QQmlComponent::statusChanged,
				this, &QQmlQuickPresenter::statusChanged);
		_latestComponent->loadUrl(viewUrl, QQmlComponent::Asynchronous);
	}
}

void QQmlQuickPresenter::statusChanged(QQmlComponent::Status status)
{
	auto component = qobject_cast<QQmlComponent*>(sender());
	if(!component)
		return;

	switch(status) {
	case QQmlComponent::Ready:
	{
		_componentCache.insert(component->url(), component);
		auto loadInfo = _loadCache.value(component);
		disconnect(component, &QQmlComponent::progressChanged,
				   this, &QQmlQuickPresenter::loadingProgressChanged);
		addObject(component, std::get<0>(loadInfo), std::get<1>(loadInfo), std::get<2>(loadInfo));
		break;
	}
	case QQmlComponent::Error:
		qmlWarning(this, component->errors()) << "Failed to load component";
		component->deleteLater();
		break;
	default:
		return;
	}

	_loadCache.remove(component);
	if(_latestComponent == component) {
		_latestComponent = nullptr;
		emit viewLoadingChanged(false);
	}
}

void QQmlQuickPresenter::addObject(QQmlComponent *component, ViewModel *viewModel, const QVariantHash &params, QPointer<ViewModel> parent)
{
	if(!_qmlPresenter) {
		qCritical() << "No QML-Presenter registered! Unable to present control of type"
					<< viewModel->metaObject()->className();
		return;
	}

	//create the view item, set initial stuff and them complete creation
	auto item = component->beginCreate(_engine->rootContext());
	if(!item) {
		qmlWarning(this) << "Unable to create quick view from the loaded component"
						 << component->url();
		return;
	}
	item->setProperty("viewModel", QVariant::fromValue(viewModel));
	viewModel->setParent(item);
	viewModel->onInit(params);
	component->completeCreate();

	auto presented = false;
	if(parent && parent->parent())
		presented = tryPresent(parent->parent(), item);
	if(!presented)
		presented = tryPresent(_qmlPresenter, item);

	if(presented)
		QQmlEngine::setObjectOwnership(item, QQmlEngine::JavaScriptOwnership);
	else {
		qmlWarning(this) << "Failed to present item for viewModel of type"
						 << viewModel->metaObject()->className();
		item->deleteLater();
	}
}

bool QQmlQuickPresenter::tryPresent(QObject *qmlPresenter, QObject *viewObject)
{
	auto meta = qmlPresenter->metaObject();
	auto index = -1;
	if(viewObject->inherits("QQuickItem"))
		index = meta->indexOfMethod("presentItem(QVariant)");
	if(viewObject->inherits("QQuickPopup"))
		index = meta->indexOfMethod("presentPopup(QVariant)");

	QVariant presented = false;
	if(index != -1) {
		meta->method(index).invoke(qmlPresenter, Qt::DirectConnection,
								   Q_RETURN_ARG(QVariant, presented),
								   Q_ARG(QVariant, QVariant::fromValue(viewObject)));
	}
	return presented.toBool();
}

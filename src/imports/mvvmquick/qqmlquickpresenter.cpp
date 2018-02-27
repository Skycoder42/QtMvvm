#include "qqmlquickpresenter.h"

#include <QtQml/QtQml>

#include <QtQuick/QQuickItem>

#include <QtQuickControls2/QQuickStyle>

#include <QtMvvmQuick/private/quickpresenter_p.h>

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QtAndroid>
#endif

using namespace QtMvvm;

QQmlQuickPresenter::QQmlQuickPresenter(QQmlEngine *engine) :
	QObject(engine),
	_engine(engine),
	_latestComponent(),
	_componentCache(),
	_loadCache()
{
	QuickPresenterPrivate::setQmlPresenter(this);
	connect(QuickPresenterPrivate::currentPresenter(), &QuickPresenter::inputViewFactoryChanged,
			this, &QQmlQuickPresenter::inputViewFactoryChanged);
}

QString QQmlQuickPresenter::currentStyle() const
{
	return QQuickStyle::name();
}

InputViewFactory *QQmlQuickPresenter::inputViewFactory() const
{
	return QuickPresenterPrivate::currentPresenter()->inputViewFactory();
}

bool QQmlQuickPresenter::isViewLoading() const
{
	return _latestComponent;
}

qreal QQmlQuickPresenter::loadingProgress() const
{
	return _latestComponent ? _latestComponent->progress() : -1.0;
}

QStringList QQmlQuickPresenter::mimeTypeFilters(const QStringList &mimeTypes) const
{
	QMimeDatabase db;
	QStringList filters;
	for(auto mime : mimeTypes) {
		auto mType = db.mimeTypeForName(mime);
		if(mType.isValid())
			filters.append(mType.filterString());
	}
	return filters;
}

void QQmlQuickPresenter::toggleDrawer()
{
	if(!_qmlPresenter) {
		qmlWarning(this).space() << "No QML-Presenter registered! Unable to toggle drawer";
		return;
	}

	QMetaObject::invokeMethod(_qmlPresenter, "toggleDrawer");
}

void QQmlQuickPresenter::popView()
{
	if(!_qmlPresenter) {
		qmlWarning(this).space() << "No QML-Presenter registered! Unable to toggle drawer";
		return;
	}

	QMetaObject::invokeMethod(_qmlPresenter, "closeAction");
}

void QQmlQuickPresenter::hapticLongPress()
{
#ifdef Q_OS_ANDROID
	QtAndroid::runOnAndroidThread([](){
		const auto content = QAndroidJniObject::getStaticField<jint>("android/R$id", "content");
		const auto LONG_PRESS = QAndroidJniObject::getStaticField<jint>("android/view/HapticFeedbackConstants", "LONG_PRESS");

		auto activity = QtAndroid::androidActivity();
		auto view = activity.callObjectMethod("findViewById",
											  "(I)Landroid/view/View;",
											  content);
		view.callMethod<jboolean>("performHapticFeedback",
								  "(I)Z",
								  LONG_PRESS);
	});
#endif
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

void QQmlQuickPresenter::showDialog(const MessageConfig &config, MessageResult *result)
{
	if(!_qmlPresenter) {
		qmlWarning(this).space() << "No QML-Presenter registered! Unable to present dialog of type"
								 << config.type();
		return;
	}

	QVariant res = false;
	QMetaObject::invokeMethod(_qmlPresenter, "showDialog", Qt::DirectConnection,
							  Q_RETURN_ARG(QVariant, res),
							  Q_ARG(QVariant, QVariant::fromValue(config)),
							  Q_ARG(QVariant, QVariant::fromValue(result)));
	if(!res.toBool()) {
		qmlWarning(this).space() << "Failed to present dialog of type"
								 << config.type();
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
		qmlWarning(this).space() << "No QML-Presenter registered! Unable to present viewModel of type"
								 << viewModel->metaObject()->className();
		return;
	}

	//create the view item, set initial stuff and them complete creation
	auto item = component->beginCreate(_engine->rootContext());
	if(!item) {
		qmlWarning(this).space() << "Unable to create quick view from the loaded component"
								 << component->url();
		return;
	}
	item->setProperty("viewModel", QVariant::fromValue(viewModel));
	viewModel->setParent(item);
	viewModel->onInit(params);
	component->completeCreate();

	auto presented = false;
	auto cPresenter = QuickPresenterPrivate::currentPresenter();
	if(parent && parent->parent())
		presented = cPresenter->presentToQml(parent->parent(), item);
	if(!presented)
		presented = cPresenter->presentToQml(_qmlPresenter, item);

	if(presented) {
		if(!item->parent())
			QQmlEngine::setObjectOwnership(item, QQmlEngine::JavaScriptOwnership);
	} else {
		qmlWarning(this).space() << "Failed to present item for viewModel of type"
								 << viewModel->metaObject()->className();
		item->deleteLater();
	}
}

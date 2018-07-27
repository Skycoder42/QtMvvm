#include "qqmlquickpresenter.h"

#include <QtQml/QtQml>

#include <QtQuick/QQuickItem>

#include <QtQuickControls2/QQuickStyle>

#include <QtMvvmCore/private/qtmvvm_logging_p.h>
#include <QtMvvmQuick/private/quickpresenter_p.h>

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QtAndroid>
#endif

using namespace QtMvvm;

QQmlQuickPresenter::QQmlQuickPresenter(QQmlEngine *engine) :
	QObject{engine},
	_engine{engine}
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
	filters.reserve(mimeTypes.size());
	for(const auto &mime : mimeTypes) {
		auto mType = db.mimeTypeForName(mime);
		if(mType.isValid())
			filters.append(mType.filterString());
	}
	return filters;
}

QColor QQmlQuickPresenter::accentTextColor(const QColor &accentColor, const QColor &baseColor) const
{
	auto a = (0.299 * accentColor.redF() + 0.587 * accentColor.greenF() + 0.144 * accentColor.blueF());
	if(baseColor.isValid()) {
		auto b = (0.299 * baseColor.redF() + 0.587 * baseColor.greenF() + 0.144 * baseColor.blueF());
		if (std::abs(a - b) >= 0.5)
			return baseColor;
	}
	if(a < 0.5)
		return Qt::white;
	else
		return Qt::black;
}

void QQmlQuickPresenter::toggleDrawer()
{
	if(!_qmlPresenter) {
		logWarning() << "No QML-Presenter registered! Unable to toggle drawer";
		return;
	}

	if(!QMetaObject::invokeMethod(_qmlPresenter, "toggleDrawer"))
		logWarning() << "QML-Presenter does not have a \"toggleDrawer\" method";
}

void QQmlQuickPresenter::popView()
{
	if(!_qmlPresenter) {
		logWarning() << "No QML-Presenter registered! Unable to pop view";
		return;
	}

	if(!QMetaObject::invokeMethod(_qmlPresenter, "closeAction"))
		logWarning() << "QML-Presenter does not have a \"closeAction\" method";
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
#else
	logDebug() << Q_FUNC_INFO;
#endif
}

void QQmlQuickPresenter::present(ViewModel *viewModel, const QVariantHash &params, const QUrl &viewUrl, QPointer<ViewModel> parent)
{
	auto component = _componentCache.object(viewUrl);
	if(component) {
		_loadQueue.enqueue(std::make_tuple(*component, viewModel, params, parent));
		processShowQueue();
	} else {
		//create component (and replace latest)
		if(_latestComponent) {
			disconnect(_latestComponent, &QQmlComponent::progressChanged,
					   this, &QQmlQuickPresenter::loadingProgressChanged);
		}
		_latestComponent = new QQmlComponent{_engine};
		component = new QSharedPointer<QQmlComponent>{_latestComponent.data()};
		_loadQueue.enqueue(std::make_tuple(*component, viewModel, params, parent));
		_componentCache.insert(viewUrl, component);

		//setup ui status
		emit viewLoadingChanged(true);
		emit loadingProgressChanged(0.0);
		connect(_latestComponent, &QQmlComponent::progressChanged,
				this, &QQmlQuickPresenter::loadingProgressChanged);
		connect(_latestComponent, &QQmlComponent::statusChanged,
				this, &QQmlQuickPresenter::statusChanged);
		_latestComponent->loadUrl(viewUrl, QQmlComponent::PreferSynchronous);
	}
}

void QQmlQuickPresenter::showDialog(const MessageConfig &config, MessageResult *result)
{
	if(!_qmlPresenter) {
		logWarning() << "No QML-Presenter registered! Unable to present dialog of type"
					 << config.type();
		return;
	}

	QVariant res = false;
	QMetaObject::invokeMethod(_qmlPresenter, "showDialog", Qt::DirectConnection,
							  Q_RETURN_ARG(QVariant, res),
							  Q_ARG(QVariant, QVariant::fromValue(config)),
							  Q_ARG(QVariant, QVariant::fromValue(result)));
	if(!res.toBool()) {
		logWarning() << "Failed to present dialog of type"
					 << config.type();
		result->complete(MessageConfig::NoButton);
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
		logDebug() << "Loaded and cached component" << component->url();
		break;
	}
	case QQmlComponent::Error:
	{
		logWarning().noquote() << "Failed to load component" << component->url()
							   << "with error:" << component->errorString().trimmed();
		_componentCache.remove(component->url());
		break;
	}
	default:
		return; //not break. code after must not be executed in this case
	}

	if(_latestComponent == component) {
		disconnect(component, &QQmlComponent::progressChanged,
				   this, &QQmlQuickPresenter::loadingProgressChanged);
		_latestComponent = nullptr;
		emit viewLoadingChanged(false);
	}
	processShowQueue();
}

void QQmlQuickPresenter::processShowQueue()
{
	while(!_loadQueue.isEmpty()) {
		auto loadInfo = _loadQueue.head();
		switch(std::get<0>(loadInfo)->status()){
		case QQmlComponent::Ready:
			addObject(std::get<0>(loadInfo).data(), std::get<1>(loadInfo), std::get<2>(loadInfo), std::get<3>(loadInfo));
			break;
		case QQmlComponent::Null:
		case QQmlComponent::Error:
			std::get<1>(loadInfo)->deleteLater();
			break;
		default:
			return; //not break. code after must not be executed in this case
		}
		_loadQueue.dequeue();
	}
}

void QQmlQuickPresenter::addObject(QQmlComponent *component, ViewModel *viewModel, const QVariantHash &params, const QPointer<ViewModel> &parent)
{
	if(!_qmlPresenter) {
		logWarning() << "No QML-Presenter registered! Unable to present viewModel of type"
					 << viewModel->metaObject()->className();
		return;
	}

	//create the view item, set initial stuff and them complete creation
	auto item = component->beginCreate(_engine->rootContext());
	if(!item) {
		logWarning() << "Unable to create quick view from the loaded component"
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
		logDebug() << "Presented" << viewModel->metaObject()->className()
				   << "with view" << item->metaObject()->className();
	} else {
		logWarning() << "Failed to present item for viewModel of type"
					 << viewModel->metaObject()->className();
		item->deleteLater();
	}
}

#include "quickpresenter.h"
#include "quickpresenter_p.h"

#include <limits>

#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QMetaMethod>

#include <QtMvvmCore/exception.h>

#include <QtQml/qqml.h>

#include <QtMvvmCore/private/qtmvvm_logging_p.h>

#include <qurlvalidator.h>

static void qtMvvmQuickInit()
{
	qmlRegisterType<QUrlValidator>("de.skycoder42.QtMvvm.Quick.Private", 1, 0, "UrlValidator");
	QtMvvm::ServiceRegistry::instance()->registerObject<QtMvvm::InputViewFactory>(QtMvvm::ServiceRegistry::DestroyOnAppDestroy, true);
	QtMvvm::ServiceRegistry::instance()->registerInterface<QtMvvm::IPresenter, QtMvvm::QuickPresenter>(QtMvvm::ServiceRegistry::DestroyOnAppDestroy, true);
}

static void initResources()
{
#ifdef QT_STATIC
	qtMvvmQuickInit();
	Q_INIT_RESOURCE(qtmvvmquick_module);
#endif
}

Q_COREAPP_STARTUP_FUNCTION(qtMvvmQuickInit)

using namespace QtMvvm;

QuickPresenter::QuickPresenter(QObject *parent) :
	QObject(parent),
	IPresenter(),
	d(new QuickPresenterPrivate())
{
	initResources();
}

QuickPresenter::~QuickPresenter() = default;

void QuickPresenter::addViewSearchDir(const QString &dirPath)
{
	QuickPresenterPrivate::currentPresenter()->d->searchDirs.prepend(dirPath);
}

void QuickPresenter::registerViewExplicitly(const QMetaObject *viewModelType, const QUrl &viewUrl)
{
	QuickPresenterPrivate::currentPresenter()->d->explicitMappings.insert(viewModelType, viewUrl);
}

InputViewFactory *QuickPresenter::getInputViewFactory()
{
	return QuickPresenterPrivate::currentPresenter()->inputViewFactory();
}

void QuickPresenter::present(QtMvvm::ViewModel *viewModel, const QVariantHash &params, QPointer<QtMvvm::ViewModel> parent)
{
	if(d->qmlPresenter) {
		auto url = findViewUrl(viewModel->metaObject());
		logDebug() << "Handing over viewModel" << viewModel->metaObject()->className() << "to QML presenter";
		QMetaObject::invokeMethod(d->qmlPresenter, "present",
								  Q_ARG(QtMvvm::ViewModel*, viewModel),
								  Q_ARG(QVariantHash, params),
								  Q_ARG(QUrl, url),
								  Q_ARG(QPointer<QtMvvm::ViewModel>, parent));
	} else
		throw PresenterException("QML presenter not ready - cannot present yet");
}

void QuickPresenter::showDialog(const QtMvvm::MessageConfig &config, QtMvvm::MessageResult *result)
{
	if(d->qmlPresenter) {
		logDebug() << "Handing over dialog of type" << config.type() << "to QML presenter";
		QMetaObject::invokeMethod(d->qmlPresenter, "showDialog",
								  Q_ARG(QtMvvm::MessageConfig, config),
								  Q_ARG(QtMvvm::MessageResult*, result));
	} else
		throw PresenterException("QML presenter not ready - cannot present yet");
}

bool QuickPresenter::presentToQml(QObject *qmlPresenter, QObject *viewObject)
{
	auto meta = qmlPresenter->metaObject();
	auto index = presentMethodIndex(meta, viewObject);
	QVariant presented = false;
	if(index != -1) {
		meta->method(index).invoke(qmlPresenter, Qt::DirectConnection,
								   Q_RETURN_ARG(QVariant, presented),
								   Q_ARG(QVariant, QVariant::fromValue(viewObject)));
	}
	return presented.toBool();
}

InputViewFactory *QuickPresenter::inputViewFactory() const
{
	return d->inputViewFactory;
}

void QuickPresenter::setInputViewFactory(InputViewFactory *inputViewFactory)
{
	d->inputViewFactory = inputViewFactory;
	emit inputViewFactoryChanged(inputViewFactory, {});
}

QUrl QuickPresenter::findViewUrl(const QMetaObject *viewModelType)
{
	Q_ASSERT(viewModelType);
	auto currentMeta = viewModelType;
	while(currentMeta &&
		  currentMeta->inherits(&ViewModel::staticMetaObject) &&
		  currentMeta != &ViewModel::staticMetaObject) {
		if(d->explicitMappings.contains(currentMeta))
			return d->explicitMappings.value(currentMeta);
		else {
			QByteArray cName = currentMeta->className();
			//strip viewmodel
			auto lIndex = cName.lastIndexOf("ViewModel");
			if(lIndex > 0)
				cName.truncate(lIndex);
			//strip namespaces
			lIndex = cName.lastIndexOf("::");
			if(lIndex > 0)
				cName = cName.mid(lIndex + 2);

			QUrl resUrl;
			auto shortest = std::numeric_limits<int>::max();
			for(const auto &dir : qAsConst(d->searchDirs)) {
				QDir searchDir(dir,
							   QStringLiteral("%1*.qml").arg(QString::fromLatin1(cName)),
							   QDir::NoSort,
							   QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
				QDirIterator iterator(searchDir, QDirIterator::Subdirectories);
				while(iterator.hasNext()) {
					iterator.next();
					if(iterator.fileName().size() < shortest) {
						shortest = iterator.fileName().size();
						if(dir.startsWith(QStringLiteral(":"))) {
							resUrl.clear();
							resUrl.setScheme(QStringLiteral("qrc"));
							resUrl.setPath(iterator.filePath().mid(1)); //skip the beginning colon
						} else
							resUrl = QUrl::fromLocalFile(iterator.filePath());
					}
				}
			}
			if(resUrl.isValid()) {
				logDebug() << "Found URL for viewmodel"
						   << viewModelType->className()
						   << "as:" << resUrl;
				return resUrl;
			}
		}

		currentMeta = currentMeta->superClass();
	}

	throw PresenterException(QByteArrayLiteral("No Url to a QML View found for ") + viewModelType->className());
}

int QuickPresenter::presentMethodIndex(const QMetaObject *presenterMetaObject, QObject *viewObject)
{
	auto index = -1;
	if(viewObject->inherits("QQuickPopup")) {
		index = presenterMetaObject->indexOfMethod("presentPopup(QVariant)");
		if(index != -1) {
			logDebug() << "Presenting" << viewObject->metaObject()->className()
					   << "as popup with presenter" << presenterMetaObject->className();
		}
	} if(viewObject->inherits("QQuickItem")) {
		if(nameOrClassContains(viewObject, QStringLiteral("Drawer"))) {
			index = presenterMetaObject->indexOfMethod("presentDrawerContent(QVariant)");
			if(index != -1) {
				logDebug() << "Presenting" << viewObject->metaObject()->className()
						   << "as drawer with presenter" << presenterMetaObject->className();
			}
		}
		if(index == -1 && nameOrClassContains(viewObject, QStringLiteral("Tab"))) {
			index = presenterMetaObject->indexOfMethod("presentTab(QVariant)");
			if(index != -1) {
				logDebug() << "Presenting" << viewObject->metaObject()->className()
						   << "as tab with presenter" << presenterMetaObject->className();
			}
		}

		if(index == -1) {
			index = presenterMetaObject->indexOfMethod("presentItem(QVariant)");
			if(index != -1) {
				logDebug() << "Presenting" << viewObject->metaObject()->className()
						   << "as item with presenter" << presenterMetaObject->className();
			}
		}
	}
	return index;
}

bool QuickPresenter::nameOrClassContains(const QObject *obj, const QString &contained, Qt::CaseSensitivity caseSensitive) const
{
	if(obj->objectName().contains(contained, caseSensitive))
		return true;
	auto currentMeta = obj->metaObject();
	while(currentMeta) {
		if(QString::fromUtf8(currentMeta->className()).contains(contained, caseSensitive))
			return true;
		currentMeta = currentMeta->superClass();
	}
	return false;
}

// ------------- Private Implementation -------------

QuickPresenter *QuickPresenterPrivate::currentPresenter()
{
	try {
#ifndef Q_NO_DEBUG
		Q_ASSERT_X(dynamic_cast<QuickPresenter*>(ServiceRegistry::instance()->service<IPresenter>()),
				   Q_FUNC_INFO,
				   "Cannot register views if the current presenter does not extend QtMvvm::QuickPresenter");
#endif
		return static_cast<QuickPresenter*>(ServiceRegistry::instance()->service<IPresenter>());
	} catch (QTMVVM_EXCEPTION_BASE &e) {
		qFatal("%s", e.what());
	}
}

void QuickPresenterPrivate::setQmlPresenter(QObject *presenter)
{
	currentPresenter()->d->qmlPresenter = presenter;
}

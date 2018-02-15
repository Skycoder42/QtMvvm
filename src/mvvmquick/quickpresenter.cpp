#include "quickpresenter.h"
#include "quickpresenter_p.h"

#include <QtCore/QDir>
#include <QtCore/QDirIterator>

#include <QtMvvmCore/private/coreapp_p.h>
#include <QtMvvmCore/private/qtmvvm_logging_p.h>

using namespace QtMvvm;

QuickPresenter::QuickPresenter(QObject *parent) :
	QObject(parent),
	IPresenter(),
	d(new QuickPresenterPrivate())
{}

QuickPresenter::~QuickPresenter() {}

void QuickPresenter::addViewSearchDir(const QString &dirUrl)
{
	QuickPresenterPrivate::currentPresenter()->d->searchDirs.prepend(dirUrl);
}

void QuickPresenter::registerViewExplicitly(const QMetaObject *viewModelType, const QUrl &viewUrl)
{
	QuickPresenterPrivate::currentPresenter()->d->explicitMappings.insert(viewModelType, viewUrl);
}

void QuickPresenter::present(QtMvvm::ViewModel *viewModel, const QVariantHash &params, QPointer<QtMvvm::ViewModel> parent)
{
	auto url = findViewUrl(viewModel->metaObject());
	if(!url.isValid())
		throw PresenterException(QByteArrayLiteral("No Url to a QML View found for ") + viewModel->metaObject()->className());

	if(d->qmlPresenter) {
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
	qDebug(Q_FUNC_INFO);
}

QUrl QuickPresenter::findViewUrl(const QMetaObject *viewModelType)
{
	auto currentMeta = viewModelType;
	while(currentMeta && currentMeta->inherits(&ViewModel::staticMetaObject)) {
		if(d->explicitMappings.contains(currentMeta))
			return d->explicitMappings.value(currentMeta);
		else {
			QByteArray cName = currentMeta->className();
			auto lIndex = cName.lastIndexOf("ViewModel");
			if(lIndex > 0)
				cName.truncate(lIndex);

			for(auto dir : qAsConst(d->searchDirs)) {
				QDir searchDir(dir,
							   QStringLiteral("%1*.qml").arg(QString::fromLatin1(cName)),
							   QDir::NoSort,
							   QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
				QDirIterator iterator(searchDir, QDirIterator::Subdirectories);
				if(iterator.hasNext()) {
					iterator.next();
					QUrl resUrl;
					if(dir.startsWith(QStringLiteral(":"))) {
						resUrl.setScheme(QStringLiteral("qrc"));
						resUrl.setPath(iterator.filePath().mid(1)); //skip the beginning colon
					} else
						resUrl = QUrl::fromLocalFile(iterator.filePath());
					logDebug() << "Found URL for viewmodel"
							   << viewModelType->className()
							   << "as:" << resUrl;
					return resUrl;
				}
			}
		}

		currentMeta = currentMeta->superClass();
	}

	return QUrl();
}

// ------------- Private Implementation -------------

QuickPresenterPrivate::QuickPresenterPrivate() :
	explicitMappings(),
	searchDirs({QStringLiteral(":/qtmvvm/views")})
{}

QuickPresenter *QuickPresenterPrivate::currentPresenter()
{
	auto presenter = static_cast<QuickPresenter*>(CoreAppPrivate::dInstance()->currentPresenter());
	if(!presenter) {
		presenter = new QuickPresenter();
		CoreApp::setMainPresenter(presenter);
	}
#ifndef Q_NO_DEBUG
	Q_ASSERT_X(dynamic_cast<QuickPresenter*>(CoreAppPrivate::dInstance()->currentPresenter()),
			   Q_FUNC_INFO,
			   "Cannot register views if the current presenter does not extend QtMvvm::QuickPresenter");
#endif
	return presenter;
}

void QuickPresenterPrivate::setQmlPresenter(QObject *presenter)
{
	currentPresenter()->d->qmlPresenter = presenter;
}

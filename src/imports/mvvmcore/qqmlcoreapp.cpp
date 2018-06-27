#include "qqmlcoreapp.h"
using namespace QtMvvm;

QQmlCoreApp::QQmlCoreApp(QObject *parent) :
	QObject(parent)
{
	connect(coreApp, &CoreApp::appStarted,
			this, &QQmlCoreApp::appStarted);
}

MessageResult *QQmlCoreApp::showDialog(const MessageConfig &config)
{
	return CoreApp::showDialog(config);
}

QVariant QQmlCoreApp::safeCastInputType(const QString &type, const QVariant &value)
{
	return CoreApp::safeCastInputType(type.toUtf8(), value);
}

void QQmlCoreApp::show(const QString &viewModelName, const QVariantHash &params, ViewModel *parentViewModel)
{
	CoreApp::show(qUtf8Printable(viewModelName), params, parentViewModel);
}

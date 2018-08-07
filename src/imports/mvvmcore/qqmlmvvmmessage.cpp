#include "qqmlmvvmmessage.h"
#include <QtQml/QQmlEngine>
using namespace QtMvvm;

QQmlMvvmMessage::QQmlMvvmMessage(QJSEngine *engine, QObject *parent) :
	QObject(parent),
	_engine(engine)
{}

void QQmlMvvmMessage::information(const QString &title, const QString &text, const QJSValue &onResult, const QString &okText)
{
	if(onResult.isCallable()) {
		QtMvvm::information(title, text, this, [onResult](){
			QJSValue(onResult).call();
		}, okText);
	} else
		QtMvvm::information(title, text, okText);
}

void QQmlMvvmMessage::question(const QString &title, const QString &text, const QJSValue &onResult, const QString &yesText, const QString &noText)
{
	if(onResult.isCallable()) {
		QtMvvm::question(title, text, this, [onResult](bool res){
			QJSValue(onResult).call({res});
		}, yesText, noText);
	} else
		QtMvvm::question(title, text, yesText, noText);
}

void QQmlMvvmMessage::warning(const QString &title, const QString &text, const QJSValue &onResult, const QString &okText)
{
	if(onResult.isCallable()) {
		QtMvvm::warning(title, text, this, [onResult](){
			QJSValue(onResult).call();
		}, okText);
	} else
		QtMvvm::warning(title, text, okText);
}

void QQmlMvvmMessage::critical(const QString &title, const QString &text, const QJSValue &onResult, const QString &okText)
{
	if(onResult.isCallable()) {
		QtMvvm::critical(title, text, this, [onResult](){
			QJSValue(onResult).call();
		}, okText);
	} else
		QtMvvm::critical(title, text, okText);
}

void QQmlMvvmMessage::about(const QString &description, const QUrl &websiteUrl, const QString &licenseName, const QUrl &licenseUrl, const QString &companyName, bool addQtVersion, const QStringList &extraTopInfos, const QString &extraBottomInfos)
{
	QtMvvm::about(description, websiteUrl, licenseName, licenseUrl, companyName, addQtVersion, extraTopInfos, extraBottomInfos);
}

void QQmlMvvmMessage::getInput(const QString &title, const QString &text, const QString &inputType, const QJSValue &onResult, const QVariant &defaultValue, const QVariantMap &viewProperties, const QString &okText, const QString &cancelText)
{
	if(onResult.isCallable()) {
		auto engine = _engine;
		QtMvvm::getInput(title, text, qUtf8Printable(inputType), this, [engine, onResult](const QVariant &result){
			QJSValue(onResult).call({engine->toScriptValue(result)});
		}, defaultValue, viewProperties, okText, cancelText);
	} else
		QtMvvm::getInput(title, text, qUtf8Printable(inputType), defaultValue, viewProperties, okText, cancelText);
}

void QQmlMvvmMessage::getExistingDirectory(const QJSValue &onResult, const QString &title, const QUrl &dir)
{
	if(onResult.isCallable()) {
		auto engine = _engine;
		QtMvvm::getExistingDirectory(this, [engine, onResult](const QUrl &url){
			QJSValue(onResult).call({engine->toScriptValue(url)});
		}, title, dir);
	} else
		QtMvvm::getExistingDirectory(title, dir);
}

void QQmlMvvmMessage::getOpenFile(const QJSValue &onResult, const QString &title, const QStringList &supportedMimeTypes, const QUrl &dir)
{
	if(onResult.isCallable()) {
		auto engine = _engine;
		QtMvvm::getOpenFile(this, [engine, onResult](const QUrl &url){
			QJSValue(onResult).call({engine->toScriptValue(url)});
		}, title, supportedMimeTypes, dir);
	} else
		QtMvvm::getOpenFile(title, supportedMimeTypes, dir);
}

void QQmlMvvmMessage::getOpenFiles(const QJSValue &onResult, const QString &title, const QStringList &supportedMimeTypes, const QUrl &dir)
{
	if(onResult.isCallable()) {
		auto engine = _engine;
		QtMvvm::getOpenFiles(this, [engine, onResult](const QList<QUrl> &url){
			QJSValue(onResult).call({engine->toScriptValue(url)});
		}, title, supportedMimeTypes, dir);
	} else
		QtMvvm::getOpenFiles(title, supportedMimeTypes, dir);
}

void QQmlMvvmMessage::getSaveFile(const QJSValue &onResult, const QString &title, const QStringList &supportedMimeTypes, const QUrl &dir)
{
	if(onResult.isCallable()) {
		auto engine = _engine;
		QtMvvm::getSaveFile(this, [engine, onResult](const QUrl &url){
			QJSValue(onResult).call({engine->toScriptValue(url)});
		}, title, supportedMimeTypes, dir);
	} else
		QtMvvm::getSaveFile(title, supportedMimeTypes, dir);
}

void QtMvvm::QQmlMvvmMessage::getColor(const QJSValue &onResult, const QString &title, const QColor &color, bool argb)
{
	if(onResult.isCallable()) {
		auto engine = _engine;
		QtMvvm::getColor(this, [engine, onResult](const QColor &resColor){
			QJSValue(onResult).call({engine->toScriptValue(resColor)});
		}, title, color, argb);
	} else
		QtMvvm::getColor(title, color, argb);
}

ProgressControl *QtMvvm::QQmlMvvmMessage::showProgress(const QString &title, const QString &label, int maximum, int minimum, bool allowCancel, int value, const QString &cancelText)
{
	auto control = QtMvvm::showProgress(title, label, maximum, minimum, allowCancel, value, cancelText);
	QQmlEngine::setObjectOwnership(control, QQmlEngine::JavaScriptOwnership);
	return control;
}

ProgressControl *QtMvvm::QQmlMvvmMessage::showIndeterminateProgress(const QString &title, const QString &label, bool allowCancel, const QString &cancelText)
{
	auto control = QtMvvm::showIndeterminateProgress(title, label, allowCancel, cancelText);
	QQmlEngine::setObjectOwnership(control, QQmlEngine::JavaScriptOwnership);
	return control;
}

ProgressControl *QtMvvm::QQmlMvvmMessage::showBusy(const QString &title, const QString &label, bool allowCancel, const QString &cancelText)
{
	auto control = QtMvvm::showBusy(title, label, allowCancel, cancelText);
	QQmlEngine::setObjectOwnership(control, QQmlEngine::JavaScriptOwnership);
	return control;
}

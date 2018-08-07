#include "message.h"
#include "message_p.h"
#include "coreapp.h"
#include "qtmvvm_logging_p.h"

#include <QtCore/QtMath>
#include <QtGui/QGuiApplication>

using namespace QtMvvm;

const QByteArray MessageConfig::TypeMessageBox = "msgbox";
const QByteArray MessageConfig::TypeInputDialog = "input";
const QByteArray MessageConfig::TypeFileDialog = "file";
const QByteArray MessageConfig::TypeColorDialog = "color";
const QByteArray MessageConfig::TypeProgressDialog = "progress";

const QByteArray MessageConfig::SubTypeInformation = "information";
const QByteArray MessageConfig::SubTypeWarning = "warning";
const QByteArray MessageConfig::SubTypeCritical = "critical";
const QByteArray MessageConfig::SubTypeQuestion = "question";
const QByteArray MessageConfig::SubTypeAbout = "about";

const QByteArray MessageConfig::SubTypeDir = "dir";
const QByteArray MessageConfig::SubTypeOpenFile = "open";
const QByteArray MessageConfig::SubTypeOpenFiles = "files";
const QByteArray MessageConfig::SubTypeSaveFile = "save";

const QByteArray MessageConfig::SubTypeRgb = "rgb";
const QByteArray MessageConfig::SubTypeArgb = "argb";

const QByteArray MessageConfig::SubTypeProgress = "progress";
const QByteArray MessageConfig::SubTypeBusy = "busy";

MessageConfig::MessageConfig(const QByteArray &type, const QByteArray &subType) :
	d(new MessageConfigPrivate(type, subType))
{
	if(subType.isEmpty())
		resetSubType();
	resetButtons();
}

MessageConfig::MessageConfig(MessageConfig &&other) noexcept = default;

MessageConfig::MessageConfig(const MessageConfig &other) = default;

MessageConfig::~MessageConfig() = default;

MessageConfig &MessageConfig::operator=(const MessageConfig &other) = default;

MessageConfig &MessageConfig::operator=(MessageConfig &&other) noexcept = default;

QByteArray MessageConfig::type() const
{
	return d->type;
}

QByteArray MessageConfig::subType() const
{
	return d->subType;
}

QString MessageConfig::title() const
{
	return d->title;
}

QString MessageConfig::text() const
{
	return d->text;
}

MessageConfig::StandardButtons MessageConfig::buttons() const
{
	return d->buttons;
}

QHash<MessageConfig::StandardButton, QString> MessageConfig::buttonTexts() const
{
	return d->buttonTexts;
}

QVariant MessageConfig::defaultValue() const
{
	if(d->type == MessageConfig::TypeInputDialog)
		return CoreApp::safeCastInputType(d->subType, d->defaultValue);
	else
		return d->defaultValue;
}

QVariantMap MessageConfig::viewProperties() const
{
	return d->editProperties;
}

MessageConfig &MessageConfig::setType(const QByteArray &type)
{
	d->type = type;
	return (*this);
}

MessageConfig &MessageConfig::setSubType(const QByteArray &subType)
{
	d->subType = subType;
	return (*this);
}

MessageConfig &MessageConfig::setTitle(const QString &title)
{
	d->title = title;
	return (*this);
}

MessageConfig &MessageConfig::setText(const QString &text)
{
	d->text = text;
	return (*this);
}

MessageConfig &MessageConfig::setButtons(StandardButtons buttons)
{
	d->buttons = buttons;
	for(auto it = d->buttonTexts.begin(); it != d->buttonTexts.end();) {
		if(d->buttons.testFlag(it.key()))
			it++;
		else
			it = d->buttonTexts.erase(it);
	}
	return (*this);
}

MessageConfig &MessageConfig::addButton(StandardButton button)
{
	d->buttons |= button;
	return (*this);
}

MessageConfig &MessageConfig::setButtonTexts(const QHash<StandardButton, QString> &buttonTexts)
{
	d->buttonTexts = buttonTexts;
	for(auto it = d->buttonTexts.constBegin(); it != d->buttonTexts.constEnd(); it++)
		d->buttons |= it.key();
	return (*this);
}

MessageConfig &MessageConfig::setButtonText(MessageConfig::StandardButton button, const QString &text)
{
	d->buttons |= button;
	d->buttonTexts.insert(button, text);
	return (*this);
}

MessageConfig &MessageConfig::setDefaultValue(const QVariant &defaultValue)
{
	d->defaultValue = defaultValue;
	return (*this);
}

MessageConfig &MessageConfig::setViewProperties(const QVariantMap &editProperties)
{
	d->editProperties = editProperties;
	return (*this);
}

MessageConfig &MessageConfig::setViewProperty(const QString &key, const QVariant &value)
{
	d->editProperties.insert(key, value);
	return (*this);
}

MessageConfig &MessageConfig::resetSubType()
{
	if(d->type == TypeMessageBox)
		d->subType = SubTypeInformation;
	else if(d->type == TypeInputDialog)
		d->subType = QMetaType::typeName(QMetaType::QString);
	else
		d->subType.clear();
	return (*this);
}

MessageConfig &MessageConfig::resetButtons()
{
	if(d->type == TypeMessageBox) {
		if(d->subType == SubTypeQuestion)
			d->buttons = Yes | No;
		else if(d->subType == SubTypeAbout)
			d->buttons = Close;
		else
			d->buttons = Ok;
	} else if(d->type == TypeInputDialog ||
			  d->type == TypeColorDialog)
		d->buttons = Ok | Cancel;
	else if(d->type == TypeProgressDialog)
		d->buttons = Cancel;
	else
		d->buttons = Ok;

	d->buttonTexts.clear();
	return (*this);
}

QVariantMap MessageConfig::buttonTextsMap() const
{
	QVariantMap map;
	for(auto it = d->buttonTexts.constBegin(); it != d->buttonTexts.constEnd(); it++)
		map.insert(QString::number(it.key()), it.value());
	return map;
}

void MessageConfig::setButtonTextsMap(const QVariantMap &buttonTexts)
{
	QHash<StandardButton, QString> map;
	for(auto it = buttonTexts.constBegin(); it != buttonTexts.constEnd(); it++)
		map.insert(static_cast<StandardButton>(it.key().toInt()), it.value().toString());
	setButtonTexts(map);
}



MessageResult::MessageResult() :
	QObject(nullptr),
	d(new MessageResultPrivate())
{}

MessageResult::~MessageResult() = default;

bool MessageResult::hasResult() const
{
	QMutexLocker lock(&d->mutex);
	return d->result.isValid();
}

QVariant MessageResult::result() const
{
	QMutexLocker lock(&d->mutex);
	return d->result;
}

bool MessageResult::autoDelete() const
{
	return d->autoDelete;
}

void MessageResult::setCloseTarget(QObject *closeObject, const QString &closeMethod)
{
	Q_ASSERT_X(closeObject, Q_FUNC_INFO, "closeObject must not be null");
	auto mo = closeObject->metaObject();
	auto mIndex = mo->indexOfMethod(qUtf8Printable(closeMethod));
	if(mIndex != -1)
		setCloseTarget(closeObject, mo->method(mIndex));
	else {
		logWarning() << "Object of type" << mo->className()
					 << "has no method with signature:" << closeMethod;
	}
}

void MessageResult::setCloseTarget(QObject *closeObject, const QMetaMethod &closeMethod)
{
	Q_ASSERT_X(closeObject, Q_FUNC_INFO, "closeObject must not be null");
	QMutexLocker lock(&d->mutex);
	d->closeObject = closeObject;
	d->closeMethod = closeMethod;
	if(d->closeRequested)
		d->closeMethod.invoke(d->closeObject, Qt::QueuedConnection);
}

void MessageResult::complete(MessageConfig::StandardButton button)
{
	QMetaObject::invokeMethod(this, "dialogDone", Qt::QueuedConnection,
							  Q_ARG(QtMvvm::MessageConfig::StandardButton, button));
	QMutexLocker lock(&d->mutex);
	if(d->autoDelete)
		QMetaObject::invokeMethod(this, "deleteLater", Qt::QueuedConnection);
}

void MessageResult::discardMessage()
{
	QMutexLocker lock(&d->mutex);
	if(d->closeObject && !d->closeRequested)
		d->closeMethod.invoke(d->closeObject, Qt::QueuedConnection);
	d->closeRequested = true;
}

void MessageResult::setResult(QVariant result)
{
	QMutexLocker lock(&d->mutex);
	d->result = std::move(result);
}

void MessageResult::setAutoDelete(bool autoDelete)
{
	if (d->autoDelete == autoDelete)
		return;

	d->autoDelete = autoDelete;
	emit autoDeleteChanged(autoDelete, {});
}



ProgressControl::ProgressControl(QObject *parent) :
	QObject{parent},
	d{new ProgressControlPrivate{}}
{}

bool ProgressControl::autoDelete() const
{
	return d->autoDelete;
}

bool ProgressControl::isIndeterminate() const
{
	return d->indeterminate;
}

ProgressControl::~ProgressControl() = default;

int ProgressControl::minimum() const
{
	return d->minimum;
}

int ProgressControl::maximum() const
{
	return d->maximum;
}

int ProgressControl::progress() const
{
	return d->progress;
}

void ProgressControl::requestCancel(MessageConfig::StandardButton btn)
{
	emit canceled(btn, {});
}

void ProgressControl::notifyClosed()
{
	emit closed({});
	if(d->autoDelete)
		deleteLater();
}

void ProgressControl::close()
{
	emit closeRequested({});
}

void ProgressControl::updateLabel(const QString &text)
{
	emit changeLabel(text, {});
}

void ProgressControl::setAutoDelete(bool autoDelete)
{
	if (d->autoDelete == autoDelete)
		return;

	d->autoDelete = autoDelete;
	emit autoDeleteChanged(d->autoDelete, {});
}

void ProgressControl::setIndeterminate(bool indeterminate)
{
	if (d->indeterminate == indeterminate)
		return;

	d->indeterminate = indeterminate;
	emit indeterminateChanged(d->indeterminate, {});
}

void ProgressControl::setMinimum(int minimum)
{
	if (d->minimum == minimum)
		return;

	d->minimum = minimum;
	emit minimumChanged(d->minimum, {});
}

void ProgressControl::setMaximum(int maximum)
{
	if (d->maximum == maximum)
		return;

	d->maximum = maximum;
	emit maximumChanged(d->maximum, {});
}

void ProgressControl::setProgress(int progress)
{
	if (d->progress == progress)
		return;

	d->progress = progress;
	emit progressChanged(d->progress, {});
}

void ProgressControl::setProgress(double progressPercent)
{
	Q_ASSERT_X(progressPercent >= 0.0 && progressPercent <= 1.0, Q_FUNC_INFO, "progressPercent must be in the range [0.0,1.0]");
	setProgress(qRound((d->maximum - d->minimum) * progressPercent + d->minimum));
}

// ------------- Private Implementation -------------

QtMvvm::MessageConfigPrivate::MessageConfigPrivate(QByteArray type, QByteArray subType) :
	QSharedData(),
	type(std::move(type)),
	subType(std::move(subType))
{}

QtMvvm::MessageConfigPrivate::MessageConfigPrivate(const QtMvvm::MessageConfigPrivate &other) = default;

// ------------- Namespace methods implementation -------------

MessageResult *QtMvvm::information(const QString &title, const QString &text, const QString &okText)
{
	MessageConfig config(MessageConfig::TypeMessageBox, MessageConfig::SubTypeInformation);
	config.setTitle(title);
	config.setText(text);
	if(!okText.isNull())
		config.setButtonText(MessageConfig::Ok, okText);
	return CoreApp::showDialog(config);
}

void QtMvvm::information(const QString &title, const QString &text, QObject *scope, const std::function<void ()> &onResult, const QString &okText)
{
	auto result = information(title, text, okText);
	if(result) {
		QObject::connect(result, &MessageResult::dialogDone,
						 scope, onResult,
						 Qt::QueuedConnection);
	}
}

void QtMvvm::information(const QString &title, const QString &text, const std::function<void ()> &onResult, const QString &okText)
{
	information(title, text, CoreApp::instance(), onResult, okText);
}

MessageResult *QtMvvm::question(const QString &title, const QString &text, const QString &yesText, const QString &noText)
{
	MessageConfig config(MessageConfig::TypeMessageBox, MessageConfig::SubTypeQuestion);
	config.setTitle(title);
	config.setText(text);
	if(!yesText.isNull())
		config.setButtonText(MessageConfig::Yes, yesText);
	if(!noText.isNull())
		config.setButtonText(MessageConfig::No, noText);
	return CoreApp::showDialog(config);
}

void QtMvvm::question(const QString &title, const QString &text, QObject *scope, const std::function<void (bool)> &onResult, const QString &yesText, const QString &noText)
{
	auto result = question(title, text, yesText, noText);
	if(result) {
		QObject::connect(result, &MessageResult::dialogDone,
						 scope, [onResult](MessageConfig::StandardButton type) {
			onResult(type == MessageConfig::Yes);
		}, Qt::QueuedConnection);
	}
}

void QtMvvm::question(const QString &title, const QString &text, const std::function<void (bool)> &onResult, const QString &yesText, const QString &noText)
{
	question(title, text, CoreApp::instance(), onResult, yesText, noText);
}

MessageResult *QtMvvm::warning(const QString &title, const QString &text, const QString &okText)
{
	MessageConfig config(MessageConfig::TypeMessageBox, MessageConfig::SubTypeWarning);
	config.setTitle(title);
	config.setText(text);
	if(!okText.isNull())
		config.setButtonText(MessageConfig::Ok, okText);
	return CoreApp::showDialog(config);
}

void QtMvvm::warning(const QString &title, const QString &text, QObject *scope, const std::function<void ()> &onResult, const QString &okText)
{
	auto result = warning(title, text, okText);
	if(result) {
		QObject::connect(result, &MessageResult::dialogDone,
						 scope, onResult,
						 Qt::QueuedConnection);
	}
}

void QtMvvm::warning(const QString &title, const QString &text, const std::function<void ()> &onResult, const QString &okText)
{
	warning(title, text, CoreApp::instance(), onResult, okText);
}

MessageResult *QtMvvm::critical(const QString &title, const QString &text, const QString &okText)
{
	MessageConfig config(MessageConfig::TypeMessageBox, MessageConfig::SubTypeCritical);
	config.setTitle(title);
	config.setText(text);
	if(!okText.isNull())
		config.setButtonText(MessageConfig::Ok, okText);
	return CoreApp::showDialog(config);
}

void QtMvvm::critical(const QString &title, const QString &text, QObject *scope, const std::function<void ()> &onResult, const QString &okText)
{
	auto result = critical(title, text, okText);
	if(result) {
		QObject::connect(result, &MessageResult::dialogDone,
						 scope, onResult,
						 Qt::QueuedConnection);
	}
}

void QtMvvm::critical(const QString &title, const QString &text, const std::function<void ()> &onResult, const QString &okText)
{
	critical(title, text, CoreApp::instance(), onResult, okText);
}

MessageResult *QtMvvm::about(const QString &description, const QUrl &websiteUrl, const QString &licenseName, const QUrl &licenseUrl, const QString &companyName, bool addQtVersion, const QStringList &extraTopInfos, const QString &extraBottomInfos)
{
	static const QString pBegin = QStringLiteral("<p>");
	static const QString pEnd = QStringLiteral("</p>");
	static const QString br = QStringLiteral("<br/>");

	MessageConfig config(MessageConfig::TypeMessageBox, MessageConfig::SubTypeAbout);
	config.setViewProperty(QStringLiteral("addQtVersion"), addQtVersion);

	config.setTitle(MessageConfig::tr("%1 — Version %2")
					.arg(QGuiApplication::applicationDisplayName(), QCoreApplication::applicationVersion()));

	//create the content string:
	//basic text
	QString text = pBegin + description + pEnd;
	//qt version info + extra infos
	if(addQtVersion || !extraTopInfos.isEmpty()) {
		text += pBegin;
		if(addQtVersion) {
			auto runtimeVers = qVersion();
			QString postFix;
			if(qstrcmp(runtimeVers, QT_VERSION_STR) != 0) {
				postFix = MessageConfig::tr(" (Built with %1)")
						  .arg(QStringLiteral(QT_VERSION_STR));
			}
			text += MessageConfig::tr("Qt-Version: <a href=\"https://www.qt.io/\">%1</a>")
					.arg(QString::fromUtf8(runtimeVers)) +
					postFix;
		}
		if(!extraTopInfos.isEmpty()) {
			auto withBr = addQtVersion;
			for(const auto &info : extraTopInfos) {
				text += info + (withBr ? br : QString());
				withBr = true;
			}
		}
		text += pEnd;
	}
	//Developer info
	text += pBegin + MessageConfig::tr("Developed by: %1")
			.arg(companyName.isEmpty() ? QCoreApplication::organizationName() : companyName);
	if(websiteUrl.isValid()) {
		text += br + MessageConfig::tr("Project Website: <a href=\"%1\">%2</a>")
				.arg(QString::fromUtf8(websiteUrl.toEncoded()), websiteUrl.toString());
	}
	if(!licenseName.isEmpty()) {
		if(licenseUrl.isValid()) {
			text += br + MessageConfig::tr("License: <a href=\"%1\">%2</a>")
					.arg(QString::fromUtf8(licenseUrl.toEncoded()), licenseName);
		} else {
			text += br + MessageConfig::tr("License: %1")
					.arg(licenseName);
		}
	}
	text += pEnd;
	//extra bottom infos
	if(!extraBottomInfos.isEmpty())
		text += pBegin + extraBottomInfos + pEnd;
	//set in the config
	config.setText(text);

	return CoreApp::showDialog(config);
}

MessageResult *QtMvvm::getInput(const QString &title, const QString &text, const char *inputType, const QVariant &defaultValue, const QVariantMap &viewProperties, const QString &okText, const QString &cancelText)
{
	MessageConfig config(MessageConfig::TypeInputDialog, inputType);
	config.setTitle(title);
	config.setText(text);
	config.setDefaultValue(defaultValue);
	config.setViewProperties(viewProperties);
	if(!okText.isNull())
		config.setButtonText(MessageConfig::Ok, okText);
	if(!cancelText.isNull())
		config.setButtonText(MessageConfig::Cancel, cancelText);
	return CoreApp::showDialog(config);
}

void QtMvvm::getInput(const QString &title, const QString &text, const char *inputType, QObject *scope, const std::function<void (QVariant)> &onResult, const QVariant &defaultValue, const QVariantMap &viewProperties, const QString &okText, const QString &cancelText)
{
	auto result = getInput(title, text, inputType, defaultValue, viewProperties, okText, cancelText);
	if(result) {
		QObject::connect(result, &MessageResult::dialogDone,
						 scope, [onResult, result](MessageConfig::StandardButton type) {
			onResult(type == MessageConfig::Ok ? result->result() : QVariant());
		}, Qt::QueuedConnection);
	}
}

void QtMvvm::getInput(const QString &title, const QString &text, const char *inputType, const std::function<void (QVariant)> &onResult, const QVariant &defaultValue, const QVariantMap &viewProperties, const QString &okText, const QString &cancelText)
{
	getInput(title, text, inputType, CoreApp::instance(), onResult, defaultValue, viewProperties, okText, cancelText);
}

MessageResult *QtMvvm::getExistingDirectory(const QString &title, const QUrl &dir)
{
	MessageConfig config(MessageConfig::TypeFileDialog, MessageConfig::SubTypeDir);
	config.setTitle(title);
	config.setDefaultValue(dir);
	return CoreApp::showDialog(config);
}

void QtMvvm::getExistingDirectory(QObject *scope, const std::function<void (QUrl)> &onResult, const QString &title, const QUrl &dir)
{
	auto result = getExistingDirectory(title, dir);
	if(result) {
		QObject::connect(result, &MessageResult::dialogDone,
						 scope, [onResult, result](MessageConfig::StandardButton type) {
			onResult(type == MessageConfig::Ok ? result->result().toUrl() : QUrl());
		}, Qt::QueuedConnection);
	}
}

void QtMvvm::getExistingDirectory(const std::function<void (QUrl)> &onResult, const QString &title, const QUrl &dir)
{
	getExistingDirectory(CoreApp::instance(), onResult, title, dir);
}

MessageResult *QtMvvm::getOpenFile(const QString &title, const QStringList &supportedMimeTypes, const QUrl &dir)
{
	MessageConfig config(MessageConfig::TypeFileDialog, MessageConfig::SubTypeOpenFile);
	config.setTitle(title);
	config.setDefaultValue(dir);
	config.setViewProperty(QStringLiteral("mimeTypes"), supportedMimeTypes);
	return CoreApp::showDialog(config);
}

void QtMvvm::getOpenFile(QObject *scope, const std::function<void (QUrl)> &onResult, const QString &title, const QStringList &supportedMimeTypes, const QUrl &dir)
{
	auto result = getOpenFile(title, supportedMimeTypes, dir);
	if(result) {
		QObject::connect(result, &MessageResult::dialogDone,
						 scope, [onResult, result](MessageConfig::StandardButton type) {
			onResult(type == MessageConfig::Ok ? result->result().toUrl() : QUrl());
		}, Qt::QueuedConnection);
	}
}

void QtMvvm::getOpenFile(const std::function<void (QUrl)> &onResult, const QString &title, const QStringList &supportedMimeTypes, const QUrl &dir)
{
	getOpenFile(CoreApp::instance(), onResult, title, supportedMimeTypes, dir);
}

MessageResult *QtMvvm::getOpenFiles(const QString &title, const QStringList &supportedMimeTypes, const QUrl &dir)
{
	MessageConfig config(MessageConfig::TypeFileDialog, MessageConfig::SubTypeOpenFiles);
	config.setTitle(title);
	config.setDefaultValue(dir);
	config.setViewProperty(QStringLiteral("mimeTypes"), supportedMimeTypes);
	return CoreApp::showDialog(config);
}

void QtMvvm::getOpenFiles(QObject *scope, const std::function<void (QList<QUrl>)> &onResult, const QString &title, const QStringList &supportedMimeTypes, const QUrl &dir)
{
	auto result = getOpenFiles(title, supportedMimeTypes, dir);
	if(result) {
		QObject::connect(result, &MessageResult::dialogDone,
						 scope, [onResult, result](MessageConfig::StandardButton type) {
			onResult(type == MessageConfig::Ok ? result->result().value<QList<QUrl>>() : QList<QUrl>());
		}, Qt::QueuedConnection);
	}
}

void QtMvvm::getOpenFiles(const std::function<void (QList<QUrl>)> &onResult, const QString &title, const QStringList &supportedMimeTypes, const QUrl &dir)
{
	getOpenFiles(CoreApp::instance(), onResult, title, supportedMimeTypes, dir);
}

MessageResult *QtMvvm::getSaveFile(const QString &title, const QStringList &supportedMimeTypes, const QUrl &dir)
{
	MessageConfig config(MessageConfig::TypeFileDialog, MessageConfig::SubTypeSaveFile);
	config.setTitle(title);
	config.setDefaultValue(dir);
	config.setViewProperty(QStringLiteral("mimeTypes"), supportedMimeTypes);
	return CoreApp::showDialog(config);
}

void QtMvvm::getSaveFile(QObject *scope, const std::function<void (QUrl)> &onResult, const QString &title, const QStringList &supportedMimeTypes, const QUrl &dir)
{
	auto result = getSaveFile(title, supportedMimeTypes, dir);
	if(result) {
		QObject::connect(result, &MessageResult::dialogDone,
						 scope, [onResult, result](MessageConfig::StandardButton type) {
			onResult(type == MessageConfig::Ok ? result->result().toUrl() : QUrl());
		}, Qt::QueuedConnection);
	}
}

void QtMvvm::getSaveFile(const std::function<void (QUrl)> &onResult, const QString &title, const QStringList &supportedMimeTypes, const QUrl &dir)
{
	getSaveFile(CoreApp::instance(), onResult, title, supportedMimeTypes, dir);
}

MessageResult *QtMvvm::getColor(const QString &title, const QColor &color, bool argb)
{
	MessageConfig config(MessageConfig::TypeColorDialog, argb ? MessageConfig::SubTypeArgb : MessageConfig::SubTypeRgb);
	config.setTitle(title);
	config.setDefaultValue(color);
	return CoreApp::showDialog(config);
}

void QtMvvm::getColor(QObject *scope, const std::function<void (QColor)> &onResult, const QString &title, const QColor &color, bool argb)
{
	auto result = getColor(title, color, argb);
	if(result) {
		QObject::connect(result, &MessageResult::dialogDone,
						 scope, [onResult, result](MessageConfig::StandardButton type) {
			onResult(type == MessageConfig::Ok ? result->result().value<QColor>() : QColor{});
		}, Qt::QueuedConnection);
	}
}

void QtMvvm::getColor(const std::function<void (QColor)> &onResult, const QString &title, const QColor &color, bool argb)
{
	getColor(CoreApp::instance(), onResult, title, color, argb);
}

MessageResult *QtMvvm::showProgress(const QString &title, const QString &label, ProgressControl *control, bool allowCancel, bool isBusy, const QString &cancelText)
{
	Q_ASSERT_X(control, Q_FUNC_INFO, "control parameter must not be null");
	MessageConfig config(MessageConfig::TypeProgressDialog, isBusy ? MessageConfig::SubTypeBusy : MessageConfig::SubTypeProgress);
	config.setTitle(title);
	config.setText(label);
	config.setDefaultValue(QVariant::fromValue(control));
	config.setButtons(allowCancel ? MessageConfig::Cancel : MessageConfig::NoButton);
	if(allowCancel && !cancelText.isNull())
		config.setButtonText(MessageConfig::Cancel, cancelText);
	return CoreApp::showDialog(config);
}

ProgressControl *QtMvvm::showProgress(QObject *scope, const QString &title, const QString &label, int maximum, int minimum, bool allowCancel, int value, const QString &cancelText)
{
	auto control = new ProgressControl{scope};
	control->setMaximum(maximum);
	control->setMinimum(minimum);
	control->setProgress(value);
	showProgress(title, label, control, allowCancel, false, cancelText);
	return control;
}

ProgressControl *QtMvvm::showProgress(const QString &title, const QString &label, int maximum, int minimum, bool allowCancel, int value, const QString &cancelText)
{
	return showProgress(nullptr, title, label, maximum, minimum, allowCancel, value, cancelText);
}

ProgressControl *QtMvvm::showIndeterminateProgress(QObject *scope, const QString &title, const QString &label, bool allowCancel, const QString &cancelText)
{
	auto control = new ProgressControl{scope};
	control->setIndeterminate(true);
	showProgress(title, label, control, allowCancel, false, cancelText);
	return control;
}

ProgressControl *QtMvvm::showIndeterminateProgress(const QString &title, const QString &label, bool allowCancel, const QString &cancelText)
{
	return showIndeterminateProgress(nullptr, title, label, allowCancel, cancelText);
}

ProgressControl *QtMvvm::showBusy(QObject *scope, const QString &title, const QString &label, bool allowCancel, const QString &cancelText)
{
	auto control = new ProgressControl{scope};
	control->setIndeterminate(true);
	showProgress(title, label, control, allowCancel, true, cancelText);
	return control;
}

ProgressControl *QtMvvm::showBusy(const QString &title, const QString &label, bool allowCancel, const QString &cancelText)
{
	return showBusy(nullptr, title, label, allowCancel, cancelText);
}

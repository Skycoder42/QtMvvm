#include "message.h"
#include "message_p.h"
using namespace QtMvvm;

MessageConfig::MessageConfig(MessageType type)  :
	d(new MessageConfigPrivate(type))
{
	resetPositiveAction();
	resetNegativeAction();
	resetNeutralAction();
}

MessageConfig::MessageConfig(const MessageConfig &other) :
	d(other.d)
{}

MessageConfig::~MessageConfig() {}

MessageConfig &MessageConfig::operator=(const MessageConfig &other)
{
	d = other.d;
	return (*this);
}

MessageConfig::MessageType MessageConfig::type() const
{
	return d->type;
}

QString MessageConfig::title() const
{
	return d->title;
}

QString MessageConfig::text() const
{
	return d->text;
}

QString MessageConfig::positiveAction() const
{
	return d->positiveAction;
}

QString MessageConfig::negativeAction() const
{
	return d->negativeAction;
}

QString MessageConfig::neutralAction() const
{
	return d->neutralAction;
}

QByteArray MessageConfig::inputType() const
{
	return d->inputType;
}

QVariant MessageConfig::defaultValue() const
{
	return d->defaultValue;
}

QVariantMap MessageConfig::editProperties() const
{
	return d->editProperties;
}

void MessageConfig::setType(MessageConfig::MessageType type)
{
	d->type = type;
	resetPositiveAction();
	resetNegativeAction();
	resetNeutralAction();
}

void MessageConfig::setTitle(QString title)
{
	d->title = title;
}

void MessageConfig::setText(QString text)
{
	d->text = text;
}

void MessageConfig::setPositiveAction(QString positiveAction)
{
	d->positiveAction = positiveAction;
}

void MessageConfig::setNegativeAction(QString negativeAction)
{
	d->negativeAction = negativeAction;
}

void MessageConfig::setNeutralAction(QString neutralAction)
{
	d->neutralAction = neutralAction;
}

void MessageConfig::setInputType(QByteArray inputType)
{
	d->inputType = inputType;
}

void MessageConfig::setDefaultValue(QVariant defaultValue)
{
	d->defaultValue = defaultValue;
}

void MessageConfig::setEditProperties(QVariantMap editProperties)
{
	d->editProperties = editProperties;
}

void MessageConfig::resetPositiveAction()
{
	switch (d->type) {
	case Information:
	case Warning:
	case Critical:
	case Input:
		d->positiveAction = tr("Ok");
		break;
	case Question:
		d->positiveAction = tr("Yes");
		break;
	default:
		Q_UNREACHABLE();
		break;
	}
}

void MessageConfig::resetNegativeAction()
{
	switch (d->type) {
	case Input:
		d->negativeAction = tr("Cancel");
		break;
	case Question:
		d->negativeAction = tr("No");
		break;
	case Information:
	case Warning:
	case Critical:
		d->negativeAction.clear();
		break;
	default:
		Q_UNREACHABLE();
		break;
	}
}

void MessageConfig::resetNeutralAction()
{
	d->neutralAction.clear();
}

// ------------- Private Implementation -------------

QtMvvm::MessageConfigPrivate::MessageConfigPrivate(MessageConfig::MessageType type) :
	QSharedData(),
	type(type),
	title(),
	text(),
	positiveAction(),
	negativeAction(),
	neutralAction(),
	inputType(),
	defaultValue(),
	editProperties()
{}

QtMvvm::MessageConfigPrivate::MessageConfigPrivate(const QtMvvm::MessageConfigPrivate &other) :
	QSharedData(other),
	type(other.type),
	title(other.title),
	text(other.text),
	positiveAction(other.positiveAction),
	negativeAction(other.negativeAction),
	neutralAction(other.neutralAction),
	inputType(other.inputType),
	defaultValue(other.defaultValue),
	editProperties(other.editProperties)
{}

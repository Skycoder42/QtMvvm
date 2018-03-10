#ifndef QTMVVM_MESSAGE_P_H
#define QTMVVM_MESSAGE_P_H

#include <QtCore/QHash>
#include <QtCore/QPointer>
#include <QtCore/QMetaMethod>
#include <QtCore/QMutex>

#include "qtmvvmcore_global.h"
#include "message.h"

namespace QtMvvm {

class MessageConfigPrivate : public QSharedData
{
public:
	MessageConfigPrivate(const QByteArray &type, const QByteArray &subType);
	MessageConfigPrivate(const MessageConfigPrivate &other);

	QByteArray type;
	QByteArray subType;
	QString title;
	QString text;
	MessageConfig::StandardButtons buttons;
	QHash<MessageConfig::StandardButton, QString> buttonTexts;
	QVariant defaultValue;
	QVariantMap editProperties;
};

class MessageResultPrivate
{
public:
	QMutex mutex;
	QPointer<QObject> closeObject;
	QMetaMethod closeMethod;
	bool closeRequested = false;
	QVariant result;
	bool autoDelete = true;
};

}

#endif // QTMVVM_MESSAGE_P_H

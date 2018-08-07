#ifndef QTMVVM_MESSAGE_P_H
#define QTMVVM_MESSAGE_P_H

#include <QtCore/QHash>
#include <QtCore/QPointer>
#include <QtCore/QMetaMethod>
#include <QtCore/QMutex>
#include <QtCore/QAtomicInteger>

#include "qtmvvmcore_global.h"
#include "message.h"

namespace QtMvvm {

class MessageConfigPrivate : public QSharedData
{
public:
	MessageConfigPrivate(QByteArray type, QByteArray subType);
	MessageConfigPrivate(const MessageConfigPrivate &other);

	QByteArray type;
	QByteArray subType;
	QString title;
	QString text;
	MessageConfig::StandardButtons buttons = MessageConfig::Ok;
	QHash<MessageConfig::StandardButton, QString> buttonTexts;
	QVariant defaultValue;
	QVariantMap editProperties;
};

class MessageResultPrivate
{
	Q_DISABLE_COPY(MessageResultPrivate)

public:
	MessageResultPrivate() = default;

	QMutex mutex;
	QPointer<QObject> closeObject;
	QMetaMethod closeMethod;
	bool closeRequested = false;
	QVariant result;
	bool autoDelete = true;
};

class ProgressControlPrivate
{
	Q_DISABLE_COPY(ProgressControlPrivate)

public:
	ProgressControlPrivate() = default;

	bool autoDelete = true;
#ifdef Q_ATOMIC_INT8_IS_SUPPORTED
	QAtomicInteger<bool> indeterminate = false;
#else
	QAtomicInteger<quint16> indeterminate = false; //use 16 bit, as 8 bit is not atomic on all platforms
#endif
	QAtomicInt minimum = 0;
	QAtomicInt maximum = 100;
	QAtomicInt progress = -1;
};

}

#endif // QTMVVM_MESSAGE_P_H

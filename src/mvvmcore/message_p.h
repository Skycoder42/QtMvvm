#ifndef QTMVVM_MESSAGE_P_H
#define QTMVVM_MESSAGE_P_H

#include "qtmvvmcore_global.h"
#include "message.h"

namespace QtMvvm {

class MessageConfigPrivate : public QSharedData
{
public:
	MessageConfigPrivate(MessageConfig::MessageType type);
	MessageConfigPrivate(const MessageConfigPrivate &other);

	MessageConfig::MessageType type;
	QString title;
	QString text;
	QString positiveAction;
	QString negativeAction;
	QString neutralAction;
	QByteArray inputType;
	QVariant defaultValue;
	QVariantMap editProperties;
};

}

#endif // QTMVVM_MESSAGE_P_H

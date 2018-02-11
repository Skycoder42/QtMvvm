#ifndef QTMVVM_MESSAGE_H
#define QTMVVM_MESSAGE_H

#include <QtCore/qcoreapplication.h>
#include <QtCore/qshareddata.h>

#include "QtMvvmCore/qtmvvmcore_global.h"

namespace QtMvvm {

class MessageConfigPrivate;
class Q_MVVMCORE_EXPORT MessageConfig
{
	Q_GADGET
	Q_DECLARE_TR_FUNCTIONS(MessageConfig)

	Q_PROPERTY(MessageType type READ type WRITE setType)
	Q_PROPERTY(QString title READ title WRITE setTitle)
	Q_PROPERTY(QString text READ text WRITE setText)
	Q_PROPERTY(QString positiveAction READ positiveAction WRITE setPositiveAction RESET resetPositiveAction)
	Q_PROPERTY(QString negativeAction READ negativeAction WRITE setNegativeAction RESET resetNegativeAction)
	Q_PROPERTY(QString neutralAction READ neutralAction WRITE setNeutralAction RESET resetNeutralAction)
	Q_PROPERTY(QByteArray inputType READ inputType WRITE setInputType)
	Q_PROPERTY(QVariant defaultValue READ defaultValue WRITE setDefaultValue)
	Q_PROPERTY(QVariantMap editProperties READ editProperties WRITE setEditProperties)

public:
	enum MessageType {
		Information,
		Question,
		Warning,
		Critical,
		Input
	};
	Q_ENUM(MessageType)

	MessageConfig(MessageType type = Information);
	MessageConfig(const MessageConfig &other);
	~MessageConfig();

	MessageConfig &operator=(const MessageConfig &other);

	MessageType type() const;
	QString title() const;
	QString text() const;
	QString positiveAction() const;
	QString negativeAction() const;
	QString neutralAction() const;
	QByteArray inputType() const;
	QVariant defaultValue() const;
	QVariantMap editProperties() const;

	void setType(MessageType type);
	void setTitle(QString title);
	void setText(QString text);
	void setPositiveAction(QString positiveAction);
	void setNegativeAction(QString negativeAction);
	void setNeutralAction(QString neutralAction);
	void setInputType(QByteArray inputType);
	void setDefaultValue(QVariant defaultValue);
	void setEditProperties(QVariantMap editProperties);

	void resetPositiveAction();
	void resetNegativeAction();
	void resetNeutralAction();

private:
	QSharedDataPointer<MessageConfigPrivate> d;
};



}

Q_DECLARE_METATYPE(QtMvvm::MessageConfig)
Q_DECLARE_TYPEINFO(QtMvvm::MessageConfig, Q_MOVABLE_TYPE);

#endif // QTMVVM_MESSAGE_H

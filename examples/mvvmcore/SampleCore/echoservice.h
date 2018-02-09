#ifndef ECHOSERVICE_H
#define ECHOSERVICE_H

#include <QtCore/QObject>

class EchoService : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit EchoService(QObject *parent = nullptr);

public Q_SLOTS:
	void ping(const QString &message);

Q_SIGNALS:
	void pong(const QString &message);
};

#endif // ECHOSERVICE_H

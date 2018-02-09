#include "echoservice.h"

EchoService::EchoService(QObject *parent) :
	QObject(parent)
{}

void EchoService::ping(const QString &message)
{
	emit pong(message);
}

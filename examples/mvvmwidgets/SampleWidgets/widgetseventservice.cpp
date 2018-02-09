#include "widgetseventservice.h"

WidgetsEventService::WidgetsEventService(QObject *parent) :
	QObject(parent),
	IEventService(),
	_cnt(0),
	_events(),
	_echoService(nullptr)
{}

WidgetsEventService::WidgetsEventService(EchoService *svc, QObject *parent) :
	QObject(parent),
	IEventService(),
	_cnt(0),
	_events(),
	_echoService(svc)
{}

int WidgetsEventService::addEvent(const QString &name)
{
	QSharedPointer<QTimer> timer {
		new QTimer(this),
		&QTimer::deleteLater
	 };

	_events.insert(_cnt, timer);
	connect(timer.data(), &QTimer::timeout, this, [this, name]() {
		_echoService->ping(name);//TODO event triggered signal
	});
	timer->start(1000);

	return _cnt++;
}

void WidgetsEventService::removeEvent(int eventId)
{
	_events.remove(eventId);
}

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
{
	qtmvvm_init();
}

int WidgetsEventService::addEvent(const QString &name)
{
	QSharedPointer<QTimer> timer {
		new QTimer(this),
		&QTimer::deleteLater
	 };

	_events.insert(_cnt, timer);
	connect(timer.data(), &QTimer::timeout, this, [this, name]() {
		_echoService->ping(name);
	});
	timer->start(1000);

	return _cnt++;
}

void WidgetsEventService::removeEvent(int eventId)
{
	_events.remove(eventId);
}

void WidgetsEventService::qtmvvm_init()
{
	qDebug(Q_FUNC_INFO);
	Q_ASSERT(_echoService);
	connect(_echoService, &EchoService::pong,
			this, &WidgetsEventService::eventTriggered);
}

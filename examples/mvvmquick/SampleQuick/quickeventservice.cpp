#include "quickeventservice.h"

QuickEventService::QuickEventService(QObject *parent) :
	QObject(parent),
	IEventService(),
	_cnt(0),
	_events(),
	_echoService(nullptr)
{}

QuickEventService::QuickEventService(EchoService *svc, QObject *parent) :
	QObject(parent),
	IEventService(),
	_cnt(0),
	_events(),
	_echoService(svc)
{
	qtmvvm_init();
}

int QuickEventService::addEvent(const QString &name)
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

void QuickEventService::removeEvent(int eventId)
{
	_events.remove(eventId);
}

void QuickEventService::qtmvvm_init()
{
	qDebug(Q_FUNC_INFO);
	Q_ASSERT(_echoService);
	connect(_echoService, &EchoService::pong,
			this, &QuickEventService::eventTriggered);
}

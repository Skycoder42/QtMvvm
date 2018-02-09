#include "widgetseventservice.h"

WidgetsEventService::WidgetsEventService(QObject *parent) :
	QObject(parent),
	IEventService(),
	_cnt(0),
	_events()
{}

int WidgetsEventService::addEvent(const QString &name)
{
	QSharedPointer<QTimer> timer {
		new QTimer(this),
		&QTimer::deleteLater
	 };

	_events.insert(_cnt, timer);
	connect(timer.data(), &QTimer::timeout, this, [this, name]() {
		emit eventTriggered(name);
	});
	timer->start(1000);

	return _cnt++;
}

void WidgetsEventService::removeEvent(int eventId)
{
	_events.remove(eventId);
}

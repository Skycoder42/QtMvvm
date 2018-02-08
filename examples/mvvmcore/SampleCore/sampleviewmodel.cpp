#include "sampleviewmodel.h"

SampleViewModel::SampleViewModel(QObject *parent) :
	ViewModel(parent),
	_name(),
	_active(false),
	_events(),
	_eventService(nullptr),
	_eventId(-1)
{}

QString SampleViewModel::name() const
{
	return _name;
}

bool SampleViewModel::active() const
{
	return _active;
}

QStringList SampleViewModel::events() const
{
	return _events;
}

void SampleViewModel::setName(QString name)
{
	if (_name == name)
		return;

	_name = name;
	emit nameChanged(_name);
	if(_active && _eventService) {
		_eventService->removeEvent(_eventId);
		_eventId = _eventService->addEvent(_name);
	}
}

void SampleViewModel::setActive(bool active)
{
	if (_active == active)
		return;

	_active = active;
	if(_eventService) {
		if(_active)
			_eventId = _eventService->addEvent(_name);
		else
			_eventService->removeEvent(_eventId);
	}
	emit activeChanged(_active);
}

void SampleViewModel::clearEvents()
{
	_events.clear();
	emit eventsChanged(_events);
}

void SampleViewModel::onInit()
{
	qInfo(Q_FUNC_INFO);
	Q_ASSERT(_eventService);
	connect(dynamic_cast<QObject*>(_eventService), SIGNAL(eventTriggered(QString)),
			this, SLOT(addEvent(QString)));
}

void SampleViewModel::onDestroy()
{
	qInfo(Q_FUNC_INFO);
	dynamic_cast<QObject*>(_eventService)->disconnect(this);
}

void SampleViewModel::onShow()
{
	qInfo(Q_FUNC_INFO);
	if(_active)
		_eventId = _eventService->addEvent(_name);
}

void SampleViewModel::onClose()
{
	qInfo(Q_FUNC_INFO);
	if(_active)
		_eventService->removeEvent(_eventId);
}

void SampleViewModel::addEvent(const QString &event)
{
	_events.append(event);
	emit eventsChanged(_events);
}

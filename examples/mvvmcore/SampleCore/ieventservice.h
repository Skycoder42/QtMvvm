#ifndef IEVENTSERVICE_H
#define IEVENTSERVICE_H

#include <QtCore/QObject>

class IEventService
{
public:
	virtual int addEvent(const QString &name) = 0;
	virtual void removeEvent(int eventId) = 0;

Q_SIGNALS:
	virtual void eventTriggered(const QString &event) = 0;
};

#define IEventServiceIid "de.skycoder42.qtmvvm.sample.IEventService"
Q_DECLARE_INTERFACE(IEventService, IEventServiceIid)
Q_DECLARE_METATYPE(IEventService*)

#endif // IEVENTSERVICE_H

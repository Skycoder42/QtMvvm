#ifndef WIDGETSEVENTSERVICE_H
#define WIDGETSEVENTSERVICE_H

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QTimer>
#include <QtCore/QSharedPointer>

#include <ieventservice.h>
#include <echoservice.h>
#include <QtMvvmCore/ViewModel>

class WidgetsEventService : public QObject, public IEventService
{
	Q_OBJECT
	Q_INTERFACES(IEventService)

	QTMVVM_INJECT_PROP(EchoService*, echoService, _echoService)

public:
	Q_INVOKABLE explicit WidgetsEventService(QObject *parent = nullptr);
	explicit WidgetsEventService(EchoService* svc, QObject *parent = nullptr);

	int addEvent(const QString &name) override;
	void removeEvent(int eventId) override;

Q_SIGNALS:
	void eventTriggered(const QString &event) final;

private:
	int _cnt;
	QHash<int, QSharedPointer<QTimer>> _events;
	EchoService* _echoService;

	Q_INVOKABLE void qtmvvm_init();
};

#endif // WIDGETSEVENTSERVICE_H

#ifndef SAMPLEVIEWMODEL_H
#define SAMPLEVIEWMODEL_H

#include <QtMvvmCore/ViewModel>

#include "ieventservice.h"

class SampleViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
	Q_PROPERTY(QStringList events READ events NOTIFY eventsChanged RESET clearEvents)

	Q_PROPERTY(IEventService* eventService MEMBER _eventService WRITE setEventService)
	QTMVVM_INJECT(IEventService*, eventService)

public:
	Q_INVOKABLE explicit SampleViewModel(QObject *parent = nullptr);

	QString name() const;
	bool active() const;
	QStringList events() const;

public Q_SLOTS:
	void setName(QString name);
	void setActive(bool active);
	void clearEvents();

Q_SIGNALS:
	void nameChanged(QString name);
	void activeChanged(bool active);
	void eventsChanged(QStringList events);

protected:
	void onInit(const QVariantHash &params) override;
	void onDestroy() override;
	void onShow() override;
	void onClose() override;

private Q_SLOTS:
	void addEvent(const QString &event);

private:
	QString _name;
	bool _active;
	QStringList _events;

	IEventService* _eventService;
	int _eventId;

	void setEventService(IEventService *eventService);
};

#endif // SAMPLEVIEWMODEL_H

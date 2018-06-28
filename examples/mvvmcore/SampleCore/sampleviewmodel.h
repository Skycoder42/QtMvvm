#ifndef SAMPLEVIEWMODEL_H
#define SAMPLEVIEWMODEL_H

#include <QtCore/QStringListModel>

#include <QtMvvmCore/ViewModel>

#include "ieventservice.h"

class SampleViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
	Q_PROPERTY(QStringListModel* eventsModel READ eventsModel CONSTANT)

	QTMVVM_INJECT_PROP(IEventService*, eventService, _eventService)

public:
	static const QString KeyActive;
	static const QString KeyNames;

	Q_INVOKABLE explicit SampleViewModel(QObject *parent = nullptr);
	~SampleViewModel();

	QString name() const;
	bool active() const;
	QStringListModel *eventsModel() const;

public Q_SLOTS:
	void setName(QString name);
	void setActive(bool active);

	void showTabs();
	void showSettings();

	void getInput();
	void getFiles();
	void getColor();
	void showProgress();

	void getResult();
	void clearEvents();
	void about();

Q_SIGNALS:
	void nameChanged(QString name);
	void activeChanged(bool active);

protected:
	void onInit(const QVariantHash &params) override;
	void onResult(quint32 requestCode, const QVariant &result) override;

private Q_SLOTS:
	void addEvent(const QString &event);

private:
	static const quint32 ResCode = 42u;

	QString _name;
	bool _active;
	QStringListModel *_eventsModel;

	IEventService* _eventService;
	int _eventId;
};

#endif // SAMPLEVIEWMODEL_H

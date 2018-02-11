#include "sampleviewmodel.h"
#include <QDebug>

const QString SampleViewModel::KeyActive = QStringLiteral("active");
const QString SampleViewModel::KeyNames = QStringLiteral("names");

SampleViewModel::SampleViewModel(QObject *parent) :
	ViewModel(parent),
	_name(),
	_active(false),
	_eventsModel(new QStringListModel(this)),
	_eventService(nullptr),
	_eventId(-1)
{}

SampleViewModel::~SampleViewModel()
{
	qInfo(Q_FUNC_INFO);
	setActive(false);
}

QString SampleViewModel::name() const
{
	return _name;
}

bool SampleViewModel::active() const
{
	return _active;
}

QStringListModel *SampleViewModel::eventsModel() const
{
	return _eventsModel;
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
	_eventsModel->setStringList({});
}

void SampleViewModel::onInit(const QVariantHash &params)
{
	qInfo() << Q_FUNC_INFO << params;
	Q_ASSERT(_eventService);
	connect(dynamic_cast<QObject*>(_eventService), SIGNAL(eventTriggered(QString)),
			this, SLOT(addEvent(QString)));

	auto names = params.value(KeyNames).toStringList();
	if(!names.isEmpty())
		setName(names.join(QLatin1Char(' ')));
	setActive(params.value(KeyActive, false).toBool());
}

void SampleViewModel::addEvent(const QString &event)
{
	qDebug() << event;
	auto row = _eventsModel->rowCount();
	if(_eventsModel->insertRow(row))
		_eventsModel->setData(_eventsModel->index(row), event);
}

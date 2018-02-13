#include "sampleviewmodel.h"
#include <QDebug>
#include <QtMvvmCore/Messages>
#include "resultviewmodel.h"

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

void SampleViewModel::getInput()
{
	QtMvvm::getInput<int>(tr("Random input"),
						  tr("Enter a number:"),
						  this, [this](int res, bool ok) {
		if(ok)
			addEvent(QString::number(res));
	}, 42);
}

void SampleViewModel::getFiles()
{
	QtMvvm::getOpenFiles(this, [this](QList<QUrl> urls) {
		for(auto url : urls)
			addEvent(url.toString());
	});
}

void SampleViewModel::getResult()
{
	showForResult<ResultViewModel>(ResCode, {
									   {QStringLiteral("default"), _name}
								   });
}

void SampleViewModel::clearEvents()
{
	QtMvvm::question(tr("Clear Eventlist"),
					 tr("Do you really want to clear the eventlist?"),
					 this, [this](bool res) {
		if(res)
			_eventsModel->setStringList({});
	});
}

void SampleViewModel::about()
{
	QtMvvm::about(tr("QtMvvm sample application"),
				  QStringLiteral("https://github.com/Skycoder42/QtMvvm"),
				  tr("BSD 3 Clause"),
				  QStringLiteral("https://github.com/Skycoder42/QtMvvm/blob/master/LICENSE"));
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

void SampleViewModel::onResult(quint32 requestCode, const QVariant &result)
{
	if(requestCode == ResCode && result.isValid())
		addEvent(result.toString());
}

void SampleViewModel::addEvent(const QString &event)
{
	qDebug() << event;
	auto row = _eventsModel->rowCount();
	if(_eventsModel->insertRow(row))
		_eventsModel->setData(_eventsModel->index(row), event);
}

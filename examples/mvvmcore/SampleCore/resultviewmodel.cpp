#include "resultviewmodel.h"

ResultViewModel::ResultViewModel(QObject *parent) :
	ViewModel(parent),
	_result()
{}

QString ResultViewModel::result() const
{
	return _result;
}

void ResultViewModel::done()
{
	emit resultReady(_result);
}

void ResultViewModel::setResult(QString result)
{
	if (_result == result)
		return;

	_result = result;
	emit resultChanged(_result);
}

void ResultViewModel::onInit(const QVariantHash &params)
{
	setResult(params.value(QStringLiteral("default")).toString());
}

#include "sampleviewmodel.h"
#include <QtCore/QDebug>
#include <QtMvvmDataSyncCore/DataSyncViewModel>

SampleViewModel::SampleViewModel(QObject *parent) :
	ViewModel(parent),
	_model(new QtDataSync::DataStoreModel(this))
{
	_model->setTypeId<SampleData>();
}

QtDataSync::DataStoreModel *SampleViewModel::model() const
{
	return _model;
}

void SampleViewModel::addData(const QString &key)
{
	_model->store()->save<SampleData>({key});
}

void SampleViewModel::removeAt(int index)
{
	auto mIndex = _model->index(index);
	if(mIndex.isValid())
		_model->store()->remove<SampleData>(_model->object<SampleData>(mIndex).key);
}

void SampleViewModel::showSyncInfo()
{
	show<QtMvvm::DataSyncViewModel>();
}

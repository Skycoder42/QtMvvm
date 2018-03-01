#ifndef SAMPLEVIEWMODEL_H
#define SAMPLEVIEWMODEL_H

#include <QtCore/QStringListModel>

#include <QtMvvmCore/ViewModel>

#include <QtDataSync/DataStoreModel>

#include "sampledata.h"

class SampleViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

	Q_PROPERTY(QtDataSync::DataStoreModel* model READ model CONSTANT)

public:
	Q_INVOKABLE explicit SampleViewModel(QObject *parent = nullptr);

	QtDataSync::DataStoreModel* model() const;

public Q_SLOTS:
	void addData(const QString &key);
	void removeAt(int index);

	void showSyncInfo();

private:
	QtDataSync::DataStoreModel *_model;
};

#endif // SAMPLEVIEWMODEL_H

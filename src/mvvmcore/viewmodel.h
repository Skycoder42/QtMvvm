#ifndef QTMVVM_VIEWMODEL_H
#define QTMVVM_VIEWMODEL_H

#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>

#include "QtMvvmCore/qtmvvmcore_global.h"

namespace QtMvvm {

class ViewModelPrivate;
class Q_MVVMCORE_EXPORT ViewModel : public QObject
{
	Q_OBJECT

	Q_PROPERTY(bool deleteOnClose READ deleteOnClose WRITE setDeleteOnClose NOTIFY deleteOnCloseChanged)

public:
	explicit ViewModel(QObject *parent = nullptr);
	~ViewModel();

	virtual ViewModel *parentViewModel() const;

	bool deleteOnClose() const;

public Q_SLOTS:
	void setDeleteOnClose(bool deleteOnClose);

Q_SIGNALS:
	void deleteOnCloseChanged(bool deleteOnClose, QPrivateSignal);

protected:
	virtual void onInit();
	virtual void onDestroy();
	virtual void onShow();
	virtual void onClose();

private:
	QScopedPointer<ViewModelPrivate> d;
};

}

#endif // QTMVVM_VIEWMODEL_H

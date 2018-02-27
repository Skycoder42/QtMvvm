#ifndef QTMVVM_COREAPP_P_H
#define QTMVVM_COREAPP_P_H

#include <QtCore/QPointer>

#include "qtmvvmcore_global.h"
#include "coreapp.h"

namespace QtMvvm {

class Q_MVVMCORE_EXPORT CoreAppPrivate : public QObject
{
	Q_OBJECT
	friend class QtMvvm::CoreApp;

public:
	static QScopedPointer<CoreAppPrivate> &dInstance();

public Q_SLOTS:
	void showViewModel(const QMetaObject *metaObject,
					   const QVariantHash &params,
					   QPointer<ViewModel> parent,
					   quint32 requestCode);
	void showDialog(const QtMvvm::MessageConfig &config, QtMvvm::MessageResult *result);

private:
	CoreAppPrivate();

	static bool bootEnabled;
	static QPointer<CoreApp> instance;

	IPresenter *presenter;
};

}

#endif // QTMVVM_COREAPP_P_H

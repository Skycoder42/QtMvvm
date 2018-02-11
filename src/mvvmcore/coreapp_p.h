#ifndef QTMVVM_COREAPP_P_H
#define QTMVVM_COREAPP_P_H

#include <QtCore/QPointer>

#include "qtmvvmcore_global.h"
#include "coreapp.h"

namespace QtMvvm {

class Q_MVVMCORE_EXPORT CoreAppPrivate
{
	friend class QtMvvm::CoreApp;

public:
	static QScopedPointer<CoreAppPrivate> &dInstance();
	void showViewModel(const QMetaObject *metaObject, const QVariantHash &params, QPointer<ViewModel> parent);

	IPresenter *currentPresenter() const;

private:
	CoreAppPrivate();

	static bool bootEnabled;
	static QPointer<CoreApp> instance;

	QScopedPointer<IPresenter> presenter;
};

}

#endif // QTMVVM_COREAPP_P_H

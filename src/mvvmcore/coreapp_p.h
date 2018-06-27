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
	CoreAppPrivate();

	static QScopedPointer<CoreAppPrivate> &dInstance();

public Q_SLOTS:
	void showViewModel(const QMetaObject *metaObject,
					   const QVariantHash &params,
					   QPointer<ViewModel> parent,
					   quint32 requestCode);
	void showDialog(const QtMvvm::MessageConfig &config, QtMvvm::MessageResult *result);

private:
	static bool bootEnabled;
	static QPointer<CoreApp> instance;

	IPresenter *presenter = nullptr;
	QHash<QByteArray, int> inputTypeMapping;
	QHash<const QMetaObject*, QPointer<ViewModel>> singleInstances;

	bool isSingleton(const QMetaObject *metaObject) const;
	const QMetaObject * getContainer(const QMetaObject *metaObject) const;

	QPointer<ViewModel> showViewModelWithReturn(const QMetaObject *metaObject,
												const QVariantHash &params,
												QPointer<ViewModel> parent,
												quint32 requestCode);
};

}

#endif // QTMVVM_COREAPP_P_H

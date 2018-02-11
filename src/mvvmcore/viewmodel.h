#ifndef QTMVVM_VIEWMODEL_H
#define QTMVVM_VIEWMODEL_H

#include <type_traits>

#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qvariant.h>
#include <QtCore/qpointer.h>

#include "QtMvvmCore/qtmvvmcore_global.h"

namespace QtMvvm {

class CoreApp;

class ViewModelPrivate;
class Q_MVVMCORE_EXPORT ViewModel : public QObject
{
	Q_OBJECT

public:
	explicit ViewModel(QObject *parent = nullptr);
	~ViewModel();

public Q_SLOTS:
	virtual void onInit(const QVariantHash &params);
	virtual void onResult(quint32 requestCode, const QVariant &result);

Q_SIGNALS:
	void resultReady(quint32 requestCode, const QVariant &result);

protected:
	template <typename TViewModel>
	inline void show(const QVariantHash &params = {}) const;
	template <typename TViewModel>
	inline void showForResult(quint32 requestCode, const QVariantHash &params = {}) const;

private:
	friend class QtMvvm::CoreApp;

	QScopedPointer<ViewModelPrivate> d;

	static void showImp(const QMetaObject *mo, const QVariantHash &params, QPointer<ViewModel> parent);
	void showResultImp(quint32 requestCode, const QMetaObject *mo, const QVariantHash &params) const;
};

// ------------- Generic Implementation -------------

template<typename TViewModel>
inline void ViewModel::show(const QVariantHash &params) const
{
	static_assert(std::is_base_of<ViewModel, TViewModel>::value, "TViewModel must extend QtMvvm::ViewModel");
	showImp(&TViewModel::staticMetaObject, params, const_cast<ViewModel*>(this));
}

template<typename TViewModel>
void ViewModel::showForResult(quint32 requestCode, const QVariantHash &params) const
{
	static_assert(std::is_base_of<ViewModel, TViewModel>::value, "TViewModel must extend QtMvvm::ViewModel");
	showResultImp(requestCode, &TViewModel::staticMetaObject, params);
}

}

#endif // QTMVVM_VIEWMODEL_H

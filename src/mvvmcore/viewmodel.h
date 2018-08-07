#ifndef QTMVVM_VIEWMODEL_H
#define QTMVVM_VIEWMODEL_H

#include <type_traits>

#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qvariant.h>
#include <QtCore/qpointer.h>

#include "QtMvvmCore/qtmvvmcore_global.h"
#include "QtMvvmCore/injection.h"

namespace QtMvvm {

class CoreApp;
class CoreAppPrivate;

class ViewModelPrivate;
//! The base class for all viewmodels
class Q_MVVMCORE_EXPORT ViewModel : public QObject
{
	Q_OBJECT

public:
	//! Default constructor with parent
	explicit ViewModel(QObject *parent = nullptr);
	~ViewModel() override;

public Q_SLOTS:
	//! Called by the presenter to initialize the viewmodel
	virtual void onInit(const QVariantHash &params);
	//! Called by the presenter when a result of a showed viewmodel is ready
	virtual void onResult(quint32 requestCode, const QVariant &result);

Q_SIGNALS:
	//! Should be emitted when the viewmodels result is ready
	void resultReady(const QVariant &result);

	//! Is emitted on single instance viewmodels when they get shown again
	QTMVVM_REVISION_1 void instanceInvoked(const QVariantHash &params, QPrivateSignal);

protected:
	//! Show another viewmodel as a child of this one
	template <typename TViewModel>
	inline void show(const QVariantHash &params = {}) const;
	//! @copybrief ViewModel::show(const QVariantHash &) const
	void show(const char *viewModelName, const QVariantHash &params = {}) const;
	//! @copybrief ViewModel::show(const QVariantHash &) const
	void show(const QMetaObject *viewMetaObject, const QVariantHash &params = {}) const;
	//! Show another viewmodel as a child of this one and expect its result
	template <typename TViewModel>
	inline void showForResult(quint32 requestCode, const QVariantHash &params = {}) const;
	//! @copybrief ViewModel::showForResult(quint32, const QVariantHash &) const
	void showForResult(quint32 requestCode, const char *viewModelName, const QVariantHash &params = {}) const;
	//! @copybrief ViewModel::showForResult(quint32, const QVariantHash &) const
	void showForResult(quint32 requestCode, const QMetaObject *viewMetaObject, const QVariantHash &params = {}) const;

private:
	friend class QtMvvm::CoreApp;
	friend class QtMvvm::CoreAppPrivate;

	QScopedPointer<ViewModelPrivate> d;

	static void showImp(const QMetaObject *metaObject, const QVariantHash &params, QPointer<ViewModel> parent, quint32 requestCode = 0);
};

//! A macro that makes a viewmodel a singleton viewmodel
#define QTMVVM_SINGLETON Q_CLASSINFO("qtmvvm_singleton", "true")

//! A macro to specify the container viewmodel to be used
#define QTMVVM_CONTAINER_VM(x) Q_CLASSINFO("qtmvvm_container_viewmodel", #x)

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
	showImp(&TViewModel::staticMetaObject, params, const_cast<ViewModel*>(this), requestCode);
}

}

Q_DECLARE_METATYPE(QtMvvm::ViewModel*)

//! @file viewmodel.h The Header that defines the QtMvvm::ViewModel class
#endif // QTMVVM_VIEWMODEL_H

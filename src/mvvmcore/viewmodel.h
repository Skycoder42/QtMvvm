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

protected:
	//! Show another viewmodel as a child of this one
	template <typename TViewModel>
	inline void show(QVariantHash params = {}) const;
	//! @copybrief ViewModel::show(const QVariantHash &) const
	void show(const char *viewModelName, const QVariantHash &params = {}) const;
	//! @copybrief ViewModel::show(const QVariantHash &) const
	void show(const QMetaObject *viewMetaObject, const QVariantHash &params = {}) const;
	//! Show another viewmodel as a child of this one and expect its result
	template <typename TViewModel>
	inline void showForResult(quint32 requestCode, QVariantHash params = {}) const;
	//! @copybrief ViewModel::showForResult(quint32, const QVariantHash &) const
	void showForResult(quint32 requestCode, const char *viewModelName, const QVariantHash &params = {}) const;
	//! @copybrief ViewModel::showForResult(quint32, const QVariantHash &) const
	void showForResult(quint32 requestCode, const QMetaObject *viewMetaObject, const QVariantHash &params = {}) const;

private:
	friend class QtMvvm::CoreApp;

	QScopedPointer<ViewModelPrivate> d;

	static void showImp(const QMetaObject *metaObject, QVariantHash params, QPointer<ViewModel> parent, quint32 requestCode = 0);
};

// ------------- Generic Implementation -------------

template<typename TViewModel>
inline void ViewModel::show(QVariantHash params) const
{
	static_assert(std::is_base_of<ViewModel, TViewModel>::value, "TViewModel must extend QtMvvm::ViewModel");
	showImp(&TViewModel::staticMetaObject, std::move(params), const_cast<ViewModel*>(this));
}

template<typename TViewModel>
void ViewModel::showForResult(quint32 requestCode, QVariantHash params) const
{
	static_assert(std::is_base_of<ViewModel, TViewModel>::value, "TViewModel must extend QtMvvm::ViewModel");
	showImp(&TViewModel::staticMetaObject, std::move(params), const_cast<ViewModel*>(this), requestCode);
}

}

#endif // QTMVVM_VIEWMODEL_H

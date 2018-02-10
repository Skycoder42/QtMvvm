#ifndef QTMVVM_VIEWMODEL_H
#define QTMVVM_VIEWMODEL_H

#include <type_traits>

#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qvariant.h>

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

	template <typename TViewModel>
	inline static void show(const QVariantHash &params = {});

public Q_SLOTS:
	void setDeleteOnClose(bool deleteOnClose);

Q_SIGNALS:
	void deleteOnCloseChanged(bool deleteOnClose, QPrivateSignal);

protected:
	virtual void onInit();
	virtual void onDestroy();
	virtual void onShow();
	virtual void onClose();

	template <typename TViewModel>
	inline void showChild(const QVariantHash &params = {}) const;

private:
	QScopedPointer<ViewModelPrivate> d;

	static void showImp(const QMetaObject *mo, const QVariantHash &params, ViewModel *parent);
};

template<typename TViewModel>
inline void ViewModel::show(const QVariantHash &params)
{
	static_assert(std::is_base_of<ViewModel, TViewModel>::value, "TViewModel must extend QtMvvm::ViewModel");
	showImp(&TViewModel::staticMetaObject, params, nullptr);
}

template<typename TViewModel>
inline void ViewModel::showChild(const QVariantHash &params) const
{
	static_assert(std::is_base_of<ViewModel, TViewModel>::value, "TViewModel must extend QtMvvm::ViewModel");
	showImp(&TViewModel::staticMetaObject, params, const_cast<ViewModel*>(this));
}

}

#endif // QTMVVM_VIEWMODEL_H

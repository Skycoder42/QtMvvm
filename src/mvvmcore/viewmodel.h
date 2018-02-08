#ifndef QTMVVM_VIEWMODEL_H
#define QTMVVM_VIEWMODEL_H

#include <type_traits>

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

	template <typename TViewModel>
	static void show(QObject *parent = nullptr);

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

	static void showImp(const QMetaObject *mo, QObject *parent);
};

template<typename TViewModel>
void ViewModel::show(QObject *parent)
{
	static_assert(std::is_base_of<ViewModel, TViewModel>::value, "TViewModel must extend QtMvvm::ViewModel");
	showImp(&TViewModel::staticMetaObject, parent);
}

}

#define QTMVVM_INJECT(x) Q_CLASSINFO("inject-" #x, "true")
#define QTMVVM_INJECT_PROP(type, name, member) \
	Q_PROPERTY(type name MEMBER member) \
	QTMVVM_INJECT(name)

#endif // QTMVVM_VIEWMODEL_H

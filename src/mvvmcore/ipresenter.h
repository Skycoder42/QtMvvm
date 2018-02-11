#ifndef QTMVVM_IPRESENTER_H
#define QTMVVM_IPRESENTER_H

#include "QtMvvmCore/qtmvvmcore_global.h"
#include "QtMvvmCore/viewmodel.h"

namespace QtMvvm {

class Q_MVVMCORE_EXPORT IPresenter
{
public:
	inline virtual ~IPresenter() = default;

	virtual void present(ViewModel *viewModel, const QVariantHash &params, QPointer<ViewModel> parent = nullptr) = 0;
};

}

#define QtMvvm_IPresenterIid "de.skycoder42.qtmvvm.core.IPresenter"
Q_DECLARE_INTERFACE(QtMvvm::IPresenter, QtMvvm_IPresenterIid)
Q_DECLARE_METATYPE(QtMvvm::IPresenter*)

#endif // QTMVVM_IPRESENTER_H

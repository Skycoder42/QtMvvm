#ifndef QTMVVM_QUICKPRESENTER_H
#define QTMVVM_QUICKPRESENTER_H

#include <QtCore/qobject.h>

#include <QtMvvmCore/ipresenter.h>

#include "QtMvvmQuick/qtmvvmquick_global.h"

namespace QtMvvm {

class Q_MVVMQUICK_EXPORT QuickPresenter : public QObject, public IPresenter
{
	Q_OBJECT
	Q_INTERFACES(QtMvvm::IPresenter)

public:
	explicit QuickPresenter(QObject *parent = nullptr);

	void present(ViewModel *viewModel, const QVariantHash &params, QPointer<ViewModel> parent) override;
	void showDialog(const MessageConfig &config, MessageResult *result) override;
};

}

#endif // QTMVVM_QUICKPRESENTER_H

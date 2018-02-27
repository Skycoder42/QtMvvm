#ifndef QTMVVM_IPRESENTER_H
#define QTMVVM_IPRESENTER_H

#include <QtCore/qexception.h>

#include "QtMvvmCore/qtmvvmcore_global.h"
#include "QtMvvmCore/viewmodel.h"
#include "QtMvvmCore/message.h"

namespace QtMvvm {

class Q_MVVMCORE_EXPORT PresenterException : public QException
{
public:
	PresenterException(const QByteArray &what);

	const char *what() const noexcept override;

	void raise() const override;
	QException *clone() const override;

protected:
	PresenterException(const PresenterException * const other);

	const QByteArray _what;
};

class Q_MVVMCORE_EXPORT IPresenter
{
public:
	inline virtual ~IPresenter() = default;

	virtual void present(ViewModel *viewModel, const QVariantHash &params, QPointer<ViewModel> parent = nullptr) = 0;
	virtual void showDialog(const MessageConfig &config, MessageResult *result) = 0;
};

}

#define QtMvvm_IPresenterIid "de.skycoder42.qtmvvm.core.IPresenter"
Q_DECLARE_INTERFACE(QtMvvm::IPresenter, QtMvvm_IPresenterIid)
Q_DECLARE_METATYPE(QtMvvm::IPresenter*)

#endif // QTMVVM_IPRESENTER_H

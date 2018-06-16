#ifndef QTMVVM_IPRESENTER_H
#define QTMVVM_IPRESENTER_H

#include <QtCore/qexception.h>

#include "QtMvvmCore/qtmvvmcore_global.h"
#include "QtMvvmCore/viewmodel.h"
#include "QtMvvmCore/message.h"

namespace QtMvvm {

//! An exception to be thrown from the presenter if presenting fails
class Q_MVVMCORE_EXPORT PresenterException : public QException
{
public:
	//! Constructor with an error message
	PresenterException(const QByteArray &what);

	//! @inherit{QException::what}
	const char *what() const noexcept override;

	//! @inherit{QException::raise}
	void raise() const override;
	//! @inherit{QException::clone}
	QException *clone() const override;

protected:
	//! @private
	PresenterException(const PresenterException * const other);

	//! @private
	const QByteArray _what;
};

//! The interface for a GUI view presenter
class Q_MVVMCORE_EXPORT IPresenter
{
	Q_DISABLE_COPY(IPresenter)
public:
	inline IPresenter() = default;
	inline virtual ~IPresenter() = default;

	//! Called to present a viewmodel
	virtual void present(ViewModel *viewModel, const QVariantHash &params, QPointer<ViewModel> parent = nullptr) = 0;
	//! Called to present a dialog via a message config
	virtual void showDialog(const MessageConfig &config, MessageResult *result) = 0;
};

}

//! The IID of the QtMvvm::IPresenter class
#define QtMvvm_IPresenterIid "de.skycoder42.qtmvvm.core.IPresenter"
Q_DECLARE_INTERFACE(QtMvvm::IPresenter, QtMvvm_IPresenterIid)
Q_DECLARE_METATYPE(QtMvvm::IPresenter*)

//! @file ipresenter.h The IPresenter class header
#endif // QTMVVM_IPRESENTER_H

#ifndef QTMVVM_QUICKPRESENTER_H
#define QTMVVM_QUICKPRESENTER_H

#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qurl.h>
#include <QtCore/qmetaobject.h>

#include <QtMvvmCore/ipresenter.h>
#include <QtMvvmCore/serviceregistry.h>

#include "QtMvvmQuick/qtmvvmquick_global.h"
#include "QtMvvmQuick/inputviewfactory.h"

namespace QtMvvm {

class QuickPresenterPrivate;
//! The IPresenter implementation for the quick module
class Q_MVVMQUICK_EXPORT QuickPresenter : public QObject, public IPresenter
{
	Q_OBJECT
	Q_INTERFACES(QtMvvm::IPresenter)

	//! The factory to create input views with, as injected property
	Q_PROPERTY(InputViewFactory* inputViewFactory READ inputViewFactory WRITE setInputViewFactory NOTIFY inputViewFactoryChanged)
	QTMVVM_INJECT(InputViewFactory*, inputViewFactory)

public:
	//! Invokable constructor
	Q_INVOKABLE explicit QuickPresenter(QObject *parent = nullptr);
	~QuickPresenter() override;

	//! Register a subclass of the QuickPresenter as the active presenter for the CoreApp
	template <typename TPresenter>
	static void registerAsPresenter();

	//! Adds a directory to search for input views
	static void addViewSearchDir(const QString &dirPath);
	//! Register a view for a viewmodel to be found by the presenter
	template <typename TViewModel>
	static void registerViewExplicitly(const QUrl &viewUrl);
	//! @copybrief registerViewExplicitly()
	static void registerViewExplicitly(const QMetaObject *viewModelType, const QUrl &viewUrl);

	//! Returns the internally used input view factory
	static InputViewFactory* getInputViewFactory();

	void present(ViewModel *viewModel, const QVariantHash &params, QPointer<ViewModel> parent) override;
	void showDialog(const MessageConfig &config, MessageResult *result) override;

	//! Is called to present a view to presenter living in QML
	virtual bool presentToQml(QObject *qmlPresenter, QObject *viewObject);

	//! @readAcFn{QuickPresenter::inputViewFactory}
	InputViewFactory* inputViewFactory() const;

public Q_SLOTS:
	//! @writeAcFn{QuickPresenter::inputViewFactory}
	void setInputViewFactory(InputViewFactory* inputViewFactory);

Q_SIGNALS:
	//! @notifyAcFn{QuickPresenter::inputViewFactory}
	void inputViewFactoryChanged(InputViewFactory* inputViewFactory, QPrivateSignal);

protected:
	//! Is called to find the URL of the view to be used for the given viewmodel
	virtual QUrl findViewUrl(const QMetaObject *viewModelType);
	//! Tries to find the index of a method to present the view object on the presenter
	virtual int presentMethodIndex(const QMetaObject *presenterMetaObject, QObject *viewObject);

	//! Checks if the object name or class name contains the given string
	bool nameOrClassContains(const QObject *obj, const QString &contained, Qt::CaseSensitivity caseSensitive = Qt::CaseInsensitive) const;

private:
	friend class QtMvvm::QuickPresenterPrivate;
	QScopedPointer<QuickPresenterPrivate> d;
};

template<typename TPresenter>
void QuickPresenter::registerAsPresenter()
{
	static_assert(std::is_base_of<QuickPresenter, TPresenter>::value, "TPresenter must inherit QtMvvm::QuickPresenter!");
	ServiceRegistry::instance()->registerInterface<IPresenter, TPresenter>();
}

template<typename TViewModel>
void QuickPresenter::registerViewExplicitly(const QUrl &viewUrl)
{
	static_assert(std::is_base_of<ViewModel, TViewModel>::value, "TViewModel must inherit ViewModel!");
	registerViewExplicitly(&TViewModel::staticMetaObject, viewUrl);
}


}

#endif // QTMVVM_QUICKPRESENTER_H

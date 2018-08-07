#ifndef QTMVVM_WIDGETSPRESENTER_H
#define QTMVVM_WIDGETSPRESENTER_H

#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>

#include <QtMvvmCore/ipresenter.h>
#include <QtMvvmCore/serviceregistry.h>

#include <QtWidgets/qwidget.h>

#include "QtMvvmWidgets/qtmvvmwidgets_global.h"
#include "QtMvvmWidgets/inputwidgetfactory.h"

namespace QtMvvm {

class WidgetsPresenterPrivate;
//! The IPresenter implementation for the widgets module
class Q_MVVMWIDGETS_EXPORT WidgetsPresenter : public QObject, public IPresenter
{
	Q_OBJECT
	Q_INTERFACES(QtMvvm::IPresenter)

	//! The factory to create input widgets with, as injected property
	Q_PROPERTY(InputWidgetFactory* inputWidgetFactory READ inputWidgetFactory WRITE setInputWidgetFactory NOTIFY inputWidgetFactoryChanged)
	QTMVVM_INJECT(InputWidgetFactory*, inputWidgetFactory)

public:
	//! Invokable constructor
	Q_INVOKABLE explicit WidgetsPresenter(QObject *parent = nullptr);
	~WidgetsPresenter() override;

	//! Register a subclass of the WidgetsPresenter as the active presenter for the CoreApp
	template <typename TPresenter>
	static void registerAsPresenter();

	//! Register a view to be found by the presenter
	template <typename TView>
	static void registerView();
	//! @copybrief registerView()
	static void registerView(const QMetaObject *viewType);

	//! Register a view for a viewmodel to be found by the presenter
	template <typename TViewModel, typename TView>
	static void registerViewExplicitly();
	//! @copybrief registerViewExplicitly()
	static void registerViewExplicitly(const QMetaObject *viewModelType, const QMetaObject *viewType);

	//! Returns the internally used input widget factory
	static InputWidgetFactory* getInputWidgetFactory();

	void present(ViewModel *viewModel, const QVariantHash &params, QPointer<ViewModel> parent) override;
	void showDialog(const MessageConfig &config, MessageResult *result) override;

	//! @readAcFn{WidgetsPresenter::inputWidgetFactory}
	InputWidgetFactory* inputWidgetFactory() const;

public Q_SLOTS:
	//! @writeAcFn{WidgetsPresenter::inputWidgetFactory}
	void setInputWidgetFactory(InputWidgetFactory* inputWidgetFactory);

Q_SIGNALS:
	//! @notifyAcFn{WidgetsPresenter::inputWidgetFactory}
	void inputWidgetFactoryChanged(InputWidgetFactory* inputWidgetFactory, QPrivateSignal);

protected:
	//! Is called to find the meta object of the view to be used for the given viewmodel
	virtual const QMetaObject *findWidgetMetaObject(const QMetaObject *viewModelMetaObject);
	//! Try to present the given view on the parent view
	virtual bool tryPresent(QWidget *view, QWidget *parentView);

	//! Called to show a widget in the foreground
	virtual void showForeground(QWidget *view) const;

	//! Called to present a dialog of MessageConfig::TypeMessageBox
	virtual void presentMessageBox(const MessageConfig &config, QPointer<MessageResult> result);
	//! Called to present a dialog of MessageConfig::TypeInputDialog
	virtual void presentInputDialog(const MessageConfig &config, QPointer<MessageResult> result);
	//! Called to present a dialog of MessageConfig::TypeFileDialog
	virtual void presentFileDialog(const MessageConfig &config, QPointer<MessageResult> result);
	//! Called to present a dialog of MessageConfig::TypeColorDialog
	void presentColorDialog(const MessageConfig &config, const QPointer<MessageResult> &result); //MAJOR make virtual
	//! Called to present a dialog of MessageConfig::TypeProgressDialog
	void presentProgressDialog(const MessageConfig &config, const QPointer<MessageResult> &result); //MAJOR make virtual
	//! Called to present a dialog of a non standard MessageConfig::type
	virtual void presentOtherDialog(const MessageConfig &config, QPointer<MessageResult> result);

private:
	QScopedPointer<WidgetsPresenterPrivate> d;
};

// ------------- Generic Implementation -------------

template<typename TPresenter>
void WidgetsPresenter::registerAsPresenter()
{
	static_assert(std::is_base_of<WidgetsPresenter, TPresenter>::value, "TPresenter must inherit QtMvvm::WidgetsPresenter!");
	ServiceRegistry::instance()->registerInterface<IPresenter, TPresenter>();
}

template<typename TView>
void WidgetsPresenter::registerView()
{
	static_assert(std::is_base_of<QWidget, TView>::value, "TWidget must inherit QWidget!");
	if(false) { //compile time check for the constructor
		Q_UNREACHABLE();
		Q_UNUSED(new TView(static_cast<ViewModel*>(nullptr), static_cast<QWidget*>(nullptr)))
	}
	registerView(&TView::staticMetaObject);
}

template<typename TViewModel, typename TView>
void WidgetsPresenter::registerViewExplicitly()
{
	static_assert(std::is_base_of<QWidget, TView>::value, "TWidget must inherit QWidget!");
	static_assert(std::is_base_of<ViewModel, TViewModel>::value, "TViewModel must inherit ViewModel!");
	if(false) { //compile time check for the constructor
		Q_UNREACHABLE();
		Q_UNUSED(new TView(static_cast<ViewModel*>(nullptr), static_cast<QWidget*>(nullptr)))
	}
	registerViewExplicitly(&TViewModel::staticMetaObject, &TView::staticMetaObject);
}

}

#endif // QTMVVM_WIDGETSPRESENTER_H

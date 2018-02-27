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
class Q_MVVMWIDGETS_EXPORT WidgetsPresenter : public QObject, public IPresenter
{
	Q_OBJECT
	Q_INTERFACES(QtMvvm::IPresenter)

	Q_PROPERTY(InputWidgetFactory* inputWidgetFactory READ inputWidgetFactory WRITE setInputWidgetFactory NOTIFY inputWidgetFactoryChanged)

public:
	Q_INVOKABLE explicit WidgetsPresenter(QObject *parent = nullptr);
	~WidgetsPresenter();

	template <typename TPresenter = WidgetsPresenter>
	static void registerAsPresenter();

	template <typename TView>
	static void registerView();
	static void registerView(const QMetaObject *viewType);

	template <typename TViewModel, typename TView>
	static void registerViewExplicitly();
	static void registerViewExplicitly(const QMetaObject *viewModelType, const QMetaObject *viewType);

	void present(ViewModel *viewModel, const QVariantHash &params, QPointer<ViewModel> parent) override;
	void showDialog(const MessageConfig &config, MessageResult *result) override;

	InputWidgetFactory* inputWidgetFactory() const;

public Q_SLOTS:
	void setInputWidgetFactory(InputWidgetFactory* inputWidgetFactory);

Q_SIGNALS:
	void inputWidgetFactoryChanged(InputWidgetFactory* inputWidgetFactory);

protected:
	virtual const QMetaObject *findWidgetMetaObject(const QMetaObject *viewModelMetaObject);
	virtual bool tryPresent(QWidget *view, QWidget *parentView);

	virtual void showForeground(QWidget *view) const;

	virtual void presentMessageBox(const MessageConfig &config, QPointer<MessageResult> result);
	virtual void presentInputDialog(const MessageConfig &config, QPointer<MessageResult> result);
	virtual void presentFileDialog(const MessageConfig &config, QPointer<MessageResult> result);
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

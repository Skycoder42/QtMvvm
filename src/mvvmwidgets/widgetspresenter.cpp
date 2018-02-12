#include "widgetspresenter.h"
#include "widgetspresenter_p.h"
#include "ipresentingview.h"

#include <QtMvvmCore/private/coreapp_p.h>
#include <QtMvvmCore/private/qtmvvm_logging_p.h>

#include <QtWidgets/QDialog>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QApplication>

#include <dialogmaster.h>

using namespace QtMvvm;

WidgetsPresenter::WidgetsPresenter(QObject *parent) :
	QObject(parent),
	IPresenter(),
	d(new WidgetsPresenterPrivate())
{}

WidgetsPresenter::~WidgetsPresenter() {}

void WidgetsPresenter::registerView(const QMetaObject *viewType)
{
	Q_ASSERT_X(viewType->inherits(&QWidget::staticMetaObject), Q_FUNC_INFO, "viewType must be a QWidget class");
	WidgetsPresenterPrivate::currentPresenter()->d->implicitMappings.insert(viewType);
}

void WidgetsPresenter::registerViewExplicitly(const QMetaObject *viewModelType, const QMetaObject *viewType)
{
	Q_ASSERT_X(viewModelType->inherits(&ViewModel::staticMetaObject), Q_FUNC_INFO, "viewModelType must be a QtMvvm::ViewModel class");
	Q_ASSERT_X(viewType->inherits(&QWidget::staticMetaObject), Q_FUNC_INFO, "viewType must be a QWidget class");
	WidgetsPresenterPrivate::currentPresenter()->d->explicitMappings.insert(viewModelType->className(), viewType);
}

void WidgetsPresenter::present(ViewModel *viewModel, const QVariantHash &params, QPointer<ViewModel> parent)
{
	// find and create view
	auto viewMetaObject = findWidgetMetaObject(viewModel->metaObject());
	if(!viewMetaObject)
		throw PresenterException("Unable to find view any matching view");

	auto parentView = parent ?
						  qobject_cast<QWidget*>(parent->parent()) :
						  nullptr;
	auto view = qobject_cast<QWidget*>(viewMetaObject->newInstance(Q_ARG(QtMvvm::ViewModel*, viewModel),
																   Q_ARG(QWidget*, parentView)));
	if(!view) {
		throw PresenterException(QByteArrayLiteral("Failed to create view of type") +
								 viewMetaObject->className() +
								 QByteArrayLiteral("(did you mark the constructor as Q_INVOKABLE? "
												   "Required signature: \"Q_INVOKABLE Contructor(QtMvvm::ViewModel *, QWidget*);\")"));
	}

	// initialize viewmodel and view relationship
	viewModel->setParent(view);
	auto hasCycle = setupLifeCycle(viewModel, view);
	if(!hasCycle)
		view->setAttribute(Qt::WA_DeleteOnClose);
	viewModel->onInit(params);

	// present the view
	auto presented = false;
	auto tPresenter = dynamic_cast<IPresentingView*>(parentView);
	if(tPresenter)
		presented = tPresenter->tryPresent(view);
	if(!presented)
		presented = tryPresent(view, parentView);

	//handle the present result
	if(!presented) {
		view->deleteLater();
		throw PresenterException(QByteArrayLiteral("Unable to find a method that is able to present a view of type") +
								 viewMetaObject->className());
	}
}

void WidgetsPresenter::showDialog(const MessageConfig &config, MessageResult *result)
{
	if(config.type() == MessageConfig::TypeMessageBox)
		presentMessageBox(config, result);
	else if(config.type() == MessageConfig::TypeInputDialog)
		presentInputDialog(config, result);
	else if(config.type() == MessageConfig::TypeFileDialog)
		presentFileDialog(config, result);
	else
		presentOtherDialog(config, result);
}

const QMetaObject *WidgetsPresenter::findWidgetMetaObject(const QMetaObject *viewModelMetaObject)
{
	auto currentMeta = viewModelMetaObject;
	while(currentMeta && currentMeta->inherits(&ViewModel::staticMetaObject)) {
		QByteArray cName = currentMeta->className();
		if(d->explicitMappings.contains(cName))
			return d->explicitMappings.value(cName);
		else {
			auto lIndex = cName.lastIndexOf("ViewModel");
			if(lIndex > 0)
				cName.truncate(lIndex);
			for(auto metaObject : d->implicitMappings) {
				QByteArray vName = metaObject->className();
				if(vName.startsWith(cName))
					return metaObject;
			}
		}

		currentMeta = currentMeta->superClass();
	}

	return nullptr;
}

bool WidgetsPresenter::tryPresent(QWidget *view, QWidget *parentView)
{
	auto metaObject = view->metaObject();

	// Check if QDialog
	if(metaObject->inherits(&QDialog::staticMetaObject)) {
		qobject_cast<QDialog*>(view)->open();
		return true;
	}

	//Check if QMainWindow
	QWidget *central = nullptr;
	if(parentView && parentView->metaObject()->inherits(&QMainWindow::staticMetaObject)) {
		auto mainWindow = qobject_cast<QMainWindow*>(parentView);

		// Check if child is QDockWidget
		if(metaObject->inherits(&QDockWidget::staticMetaObject)) {
			auto dockWidget = qobject_cast<QDockWidget*>(view);
			if(!mainWindow->restoreDockWidget(dockWidget))
				mainWindow->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
			return true;
		}

		//extract central widget for futher parts
		central = mainWindow->centralWidget();
	}

	// for both, the parent and the central view, depending on which is available
	for(auto pView : { parentView, central }) {
		if(!pView)
			continue;

		// Check if QMdiArea and child is QMdiSubWindow
		if(pView->metaObject()->inherits(&QMdiArea::staticMetaObject) &&
		   metaObject->inherits(&QMdiSubWindow::staticMetaObject)) {
			auto mdiArea = qobject_cast<QMdiArea*>(pView);
			mdiArea->addSubWindow(view);
			return true;
		}
	}

	//none of the special cases -> simply "show" it
	showForeground(view);
	return true;
}

bool WidgetsPresenter::setupLifeCycle(ViewModel *viewModel, QWidget *view)
{
	//TODO implement as soon as quick part is ready as well
	Q_UNUSED(viewModel)
	Q_UNUSED(view)
	return false;
}

void WidgetsPresenter::showForeground(QWidget *view) const
{
	if(view->windowState().testFlag(Qt::WindowMinimized))
		view->setWindowState(view->windowState() & ~Qt::WindowMinimized);
	view->show();
	view->raise();
	QApplication::alert(view);
	view->activateWindow();
}

void WidgetsPresenter::presentMessageBox(const MessageConfig &config, MessageResult *result)
{
	DialogMaster::MessageBoxInfo info;
	//set the icon based of the type
	if(config.type() == MessageConfig::SubTypeInformation)
		info = DialogMaster::createInformation();
	else if(config.type() == MessageConfig::SubTypeWarning)
		info = DialogMaster::createWarning();
	else if(config.type() == MessageConfig::SubTypeCritical)
		info = DialogMaster::createCritical();
	else if(config.type() == MessageConfig::SubTypeQuestion)
		info = DialogMaster::createQuestion();
	else if(config.type() == MessageConfig::SubTypeAbout) {
		info = DialogMaster::createInformation();
		info.icon = QGuiApplication::windowIcon();
	}

	info.title = config.title();
	info.text = config.text();
	info.buttons = static_cast<QMessageBox::StandardButtons>(static_cast<int>(config.buttons())); //is ok, as the buttons are the same
	auto btns = config.buttonTexts();
	for(auto it = btns.constBegin(); it != btns.constEnd(); it++)
		info.buttonTexts.insert(static_cast<QMessageBox::StandardButton>(it.key()), it.value());

	//special properties
	bool checked = false;
	auto props = config.viewProperties(); //TODO document all these
	if(!props.value(QStringLiteral("modal"), false).toBool())
		info.parent = QApplication::activeWindow();
	if(props.contains(QStringLiteral("windowTitle")))
		info.windowTitle = props.value(QStringLiteral("windowTitle")).toString();
	if(props.contains(QStringLiteral("details")))
		info.details = props.value(QStringLiteral("details")).toString();
	if(props.contains(QStringLiteral("checkable"))) {
		if(props.value(QStringLiteral("checkable")).toBool()) {
			checked = config.defaultValue().toBool();
			info.checked = &checked;
			if(props.contains(QStringLiteral("checkString")))
				info.checkString = props.value(QStringLiteral("checkString")).toString();
		}
	}

	//show the msgbox
	int res = DialogMaster::messageBox(info);
	if(info.checked)
		result->setResult(checked);
	result->complete(static_cast<MessageConfig::StandardButton>(res));
}

void WidgetsPresenter::presentInputDialog(const MessageConfig &config, MessageResult *result)
{

}

void WidgetsPresenter::presentFileDialog(const MessageConfig &config, MessageResult *result)
{

}

void WidgetsPresenter::presentOtherDialog(const MessageConfig &config, MessageResult *result)
{
	Q_UNUSED(result)
	throw PresenterException(QByteArrayLiteral("Unable to find a method that is able to show a dialog of type") +
							 config.type());
}

// ------------- Private Implementation -------------

WidgetsPresenterPrivate::WidgetsPresenterPrivate() :
	implicitMappings(),
	explicitMappings()
{}

WidgetsPresenter *WidgetsPresenterPrivate::currentPresenter()
{
#ifndef Q_NO_DEBUG
	Q_ASSERT_X(dynamic_cast<WidgetsPresenter*>(CoreAppPrivate::dInstance()->currentPresenter()),
			   Q_FUNC_INFO,
			   "Cannot register widgets if the current presenter does not extend QtMvvm::WidgetsPresenter");
#endif
	auto presenter = static_cast<WidgetsPresenter*>(CoreAppPrivate::dInstance()->currentPresenter());
	if(!presenter) {
		presenter = new WidgetsPresenter();
		CoreApp::setMainPresenter(presenter);
	}
	return presenter;
}

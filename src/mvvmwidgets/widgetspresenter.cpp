#include "widgetspresenter.h"
#include "widgetspresenter_p.h"
#include "ipresentingview.h"

#include <QtCore/QMetaProperty>

#include <QtWidgets/QDialog>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QApplication>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>

#include <QtMvvmCore/private/coreapp_p.h>
#include <QtMvvmCore/private/qtmvvm_logging_p.h>

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
	WidgetsPresenterPrivate::currentPresenter()->d->explicitMappings.insert(viewModelType, viewType);
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

InputWidgetFactory *WidgetsPresenter::inputWidgetFactory() const
{
	return d->inputViewFactory.data();
}

void WidgetsPresenter::setInputWidgetFactory(InputWidgetFactory *inputWidgetFactory)
{
	d->inputViewFactory.reset(inputWidgetFactory);
	emit inputWidgetFactoryChanged(inputWidgetFactory);
}

const QMetaObject *WidgetsPresenter::findWidgetMetaObject(const QMetaObject *viewModelMetaObject)
{
	auto currentMeta = viewModelMetaObject;
	while(currentMeta && currentMeta->inherits(&ViewModel::staticMetaObject)) {
		if(d->explicitMappings.contains(currentMeta))
			return d->explicitMappings.value(currentMeta);
		else {
			QByteArray cName = currentMeta->className();
			auto lIndex = cName.lastIndexOf("ViewModel");
			if(lIndex > 0)
				cName.truncate(lIndex);

			auto shortest = std::numeric_limits<int>::max();
			const QMetaObject *res = nullptr;
			for(auto metaObject : d->implicitMappings) {
				QByteArray vName = metaObject->className();
				if(vName.startsWith(cName) && vName.size() < shortest) {
					shortest = vName.size();
					res = metaObject;
				}
			}
			if(res)
				return res;
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

void WidgetsPresenter::showForeground(QWidget *view) const
{
	if(view->windowState().testFlag(Qt::WindowMinimized))
		view->setWindowState(view->windowState() & ~Qt::WindowMinimized);
	view->show();
	view->raise();
	QApplication::alert(view);
	view->activateWindow();
}

void WidgetsPresenter::presentMessageBox(const MessageConfig &config, QPointer<MessageResult> result)
{
	auto qtHelp = false;
	DialogMaster::MessageBoxInfo info;
	//set the icon based of the type
	if(config.subType() == MessageConfig::SubTypeInformation)
		info = DialogMaster::createInformation();
	else if(config.subType() == MessageConfig::SubTypeWarning)
		info = DialogMaster::createWarning();
	else if(config.subType() == MessageConfig::SubTypeCritical)
		info = DialogMaster::createCritical();
	else if(config.subType() == MessageConfig::SubTypeQuestion)
		info = DialogMaster::createQuestion();
	else if(config.subType() == MessageConfig::SubTypeAbout) {
		info = DialogMaster::createInformation();
		info.windowTitle = tr("About");
		auto mIcon = QGuiApplication::windowIcon();
		if(!mIcon.isNull())
			info.icon = mIcon;
		qtHelp = config.viewProperties().value(QStringLiteral("addQtVersion"), true).toBool();
		qtHelp = config.viewProperties().value(QStringLiteral("showQtHelp"), qtHelp).toBool();//TODO document too
	}

	info.escapeButton = QMessageBox::NoButton; //use no button for non button closes
	info.title = config.title();
	info.text = config.text();
	info.buttons = static_cast<QMessageBox::StandardButtons>(static_cast<int>(config.buttons())); //is ok, as the buttons are the same
	auto btns = config.buttonTexts();
	for(auto it = btns.constBegin(); it != btns.constEnd(); it++)
		info.buttonTexts.insert(static_cast<QMessageBox::StandardButton>(it.key()), it.value());
	if(qtHelp) {
		info.buttons |= QMessageBox::Help;
		info.buttonTexts.insert(QMessageBox::Help, tr("About &Qt"));
	}

	//special properties
	QSharedPointer<bool> checked;
	auto props = config.viewProperties(); //TODO document all these
	if(!props.value(QStringLiteral("modal"), false).toBool())
		info.parent = QApplication::activeWindow();
	if(props.contains(QStringLiteral("windowTitle")))
		info.windowTitle = props.value(QStringLiteral("windowTitle")).toString();
	if(props.contains(QStringLiteral("details")))
		info.details = props.value(QStringLiteral("details")).toString();
	if(props.contains(QStringLiteral("checkable"))) {
		if(props.value(QStringLiteral("checkable")).toBool()) {
			checked = QSharedPointer<bool>::create(config.defaultValue().toBool());
			info.checked = checked.data();
			if(props.contains(QStringLiteral("checkString")))
				info.checkString = props.value(QStringLiteral("checkString")).toString();
		}
	}

	//create and show the msgbox
	auto msgBox = DialogMaster::createMessageBox(info);
	connect(msgBox, &QMessageBox::finished,
			result, [msgBox, qtHelp, checked, result](){
		int sBtn = msgBox->standardButton(msgBox->clickedButton());
		if(result) {
			if(checked)
				result->setResult(*checked);
			result->complete(static_cast<MessageConfig::StandardButton>(sBtn));
			if(qtHelp && sBtn == QMessageBox::Help)
				QApplication::aboutQt();
		}
	});
	msgBox->open();
}

void WidgetsPresenter::presentInputDialog(const MessageConfig &config, QPointer<MessageResult> result)
{
	auto input = d->inputViewFactory->createInput(config.subType(), nullptr, config.viewProperties());
	if(!input)
		return;

	QWidget *parent = nullptr;
	if(!config.viewProperties().value(QStringLiteral("modal"), false).toBool())
		parent = QApplication::activeWindow();
	auto dialog = new QDialog(parent);
	dialog->setAttribute(Qt::WA_DeleteOnClose);
	auto layout = new QVBoxLayout(dialog);
	dialog->setLayout(layout);

	//set title and text
	auto text = config.text();
	if(text.isNull())
		text = config.title();
	else
		dialog->setWindowTitle(config.title());//TODO as header better?
	if(!text.isNull()) {
		auto label = new QLabel(text, dialog);
		layout->addWidget(label);
	}

	//add the input
	input->setParent(dialog);
	auto property = input->metaObject()->userProperty();
	property.write(input, config.defaultValue());
	layout->addWidget(input);

	//add the buttons
	auto btnBox = new QDialogButtonBox(dialog);
	btnBox->setStandardButtons(static_cast<QDialogButtonBox::StandardButtons>(static_cast<int>(config.buttons()))); //is ok, as the buttons are the same
	auto btns = config.buttonTexts();
	for(auto it = btns.constBegin(); it != btns.constEnd(); it++){
		auto sBtn = static_cast<QDialogButtonBox::StandardButton>(it.key());
		btnBox->addButton(sBtn);
		btnBox->button(sBtn)->setText(it.value());
	}
	layout->addWidget(btnBox);

	//connect stuff
	QObject::connect(btnBox, &QDialogButtonBox::clicked,
					 dialog, [dialog, btnBox](QAbstractButton *btn){
		dialog->done(btnBox->standardButton(btn));
	});
	QObject::connect(dialog, &QDialog::finished,
					 dialog, [dialog, input, property, result](int resCode){
		if(result) {
			result->complete(static_cast<MessageConfig::StandardButton>(resCode),
							 property.read(input));
		}
	});

	//finalize and show
	dialog->adjustSize();
	DialogMaster::masterDialog(dialog);
	dialog->open();
}

void WidgetsPresenter::presentFileDialog(const MessageConfig &config, QPointer<MessageResult> result)
{
	auto props = config.viewProperties();

	QWidget *parent = nullptr;
	if(!props.value(QStringLiteral("modal"), false).toBool())
		parent = QApplication::activeWindow();
	auto dialog = new QFileDialog(parent);

	//prepare the dialog
	auto title = config.title();
	if(!title.isNull())
		dialog->setWindowTitle(title);
	auto dirUrl = config.defaultValue().toUrl();
	if(dirUrl.isValid())
		dialog->setDirectoryUrl(dirUrl);

	//set the file mode
	auto isMultiFile = false;
	if(config.subType() == MessageConfig::SubTypeDir) {
		dialog->setAcceptMode(QFileDialog::AcceptOpen);
		dialog->setFileMode(QFileDialog::Directory);
	} else if(config.subType() == MessageConfig::SubTypeOpenFile) {
		dialog->setAcceptMode(QFileDialog::AcceptOpen);
		dialog->setFileMode(QFileDialog::ExistingFile);
	} else if(config.subType() == MessageConfig::SubTypeOpenFiles) {
		isMultiFile = true;
		dialog->setAcceptMode(QFileDialog::AcceptOpen);
		dialog->setFileMode(QFileDialog::ExistingFiles);
	} else if(config.subType() == MessageConfig::SubTypeSaveFile) {
		dialog->setAcceptMode(QFileDialog::AcceptSave);
		dialog->setFileMode(QFileDialog::AnyFile);
	}

	//set extra props
	for(auto it = props.constBegin(); it != props.constEnd(); it++)
		dialog->setProperty(qUtf8Printable(it.key()), it.value());

	//connect stuff
	QObject::connect(dialog, &QDialog::finished,
					 dialog, [dialog, isMultiFile, result](int resCode){
		if(result) {
			if(isMultiFile)
				result->setResult(QVariant::fromValue(dialog->selectedUrls()));
			else
				result->setResult(dialog->selectedUrls().first());
			if(resCode == QDialog::Accepted)
				result->complete(MessageConfig::Ok);
			else
				result->complete(MessageConfig::Cancel);
		}
	});

	//finalize and show
	dialog->adjustSize();
	DialogMaster::masterDialog(dialog);
	dialog->open();
}

void WidgetsPresenter::presentOtherDialog(const MessageConfig &config, QPointer<MessageResult> result)
{
	Q_UNUSED(result)
	throw PresenterException(QByteArrayLiteral("Unable to find a method that is able to show a dialog of type") +
							 config.type());
}

// ------------- Private Implementation -------------

WidgetsPresenterPrivate::WidgetsPresenterPrivate() :
	inputViewFactory(new InputWidgetFactory()),
	implicitMappings(),
	explicitMappings()
{}

WidgetsPresenter *WidgetsPresenterPrivate::currentPresenter()
{
	auto presenter = static_cast<WidgetsPresenter*>(CoreAppPrivate::dInstance()->currentPresenter());
	if(!presenter) {
		presenter = new WidgetsPresenter();
		CoreApp::setMainPresenter(presenter);
	}
#ifndef Q_NO_DEBUG
	Q_ASSERT_X(dynamic_cast<WidgetsPresenter*>(CoreAppPrivate::dInstance()->currentPresenter()),
			   Q_FUNC_INFO,
			   "Cannot register widgets if the current presenter does not extend QtMvvm::WidgetsPresenter");
#endif
	return presenter;
}

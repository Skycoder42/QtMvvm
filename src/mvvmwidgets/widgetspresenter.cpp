#include "widgetspresenter.h"
#include "widgetspresenter_p.h"
#include "ipresentingview.h"
#include "settingsdialog.h"
#include "progressdialog_p.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QCoreApplication>

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
#include <QtWidgets/QColorDialog>

#include <QtMvvmCore/CoreApp>
#include <QtMvvmCore/exception.h>
#include <QtMvvmCore/private/qtmvvm_logging_p.h>

#include <dialogmaster.h>
#include <qurlvalidator.h>

static void qtMvvmWidgetsInit()
{
	QtMvvm::ServiceRegistry::instance()->registerObject<QtMvvm::InputWidgetFactory>(QtMvvm::ServiceRegistry::DestroyOnAppDestroy, true);
	QtMvvm::ServiceRegistry::instance()->registerInterface<QtMvvm::IPresenter, QtMvvm::WidgetsPresenter>(QtMvvm::ServiceRegistry::DestroyOnAppDestroy, true);
}

static void initResources()
{
#ifdef QT_STATIC
	qtMvvmWidgetsInit();
	Q_INIT_RESOURCE(qtmvvmwidgets_module);
#endif
}

Q_COREAPP_STARTUP_FUNCTION(qtMvvmWidgetsInit)

using namespace QtMvvm;

WidgetsPresenter::WidgetsPresenter(QObject *parent) :
	QObject(parent),
	IPresenter(),
	d(new WidgetsPresenterPrivate())
{
	initResources();
}

WidgetsPresenter::~WidgetsPresenter() = default;

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

InputWidgetFactory *WidgetsPresenter::getInputWidgetFactory()
{
	return WidgetsPresenterPrivate::currentPresenter()->inputWidgetFactory();
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
		throw PresenterException(QByteArrayLiteral("Failed to create view of type \"") +
								 viewMetaObject->className() +
								 QByteArrayLiteral("\" (did you mark the constructor as Q_INVOKABLE? "
												   "Required signature: \"Q_INVOKABLE Contructor(QtMvvm::ViewModel *, QWidget*);\")"));
	}

	// initialize viewmodel and view relationship
	viewModel->setParent(view);
	view->setAttribute(Qt::WA_DeleteOnClose);
	viewModel->onInit(params);

	// present the view and handle the present result
	if(!tryPresent(view, parentView)) {
		view->deleteLater();
		throw PresenterException(QByteArrayLiteral("Unable to find a method that is able to present a view of type") +
								 viewMetaObject->className());
	} else {
		logDebug() << "Presented" << viewModel->metaObject()->className()
				   << "with view" << viewMetaObject->className();
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
	else if(config.type() == MessageConfig::TypeColorDialog)
		presentColorDialog(config, result);
	else if(config.type() == MessageConfig::TypeProgressDialog)
		presentProgressDialog(config, result);
	else
		presentOtherDialog(config, result);
	logDebug() << "Presented dialog of type" << config.type();
}

InputWidgetFactory *WidgetsPresenter::inputWidgetFactory() const
{
	return d->inputViewFactory;
}

void WidgetsPresenter::setInputWidgetFactory(InputWidgetFactory *inputWidgetFactory)
{
	d->inputViewFactory = inputWidgetFactory;
	emit inputWidgetFactoryChanged(inputWidgetFactory, {});
}

const QMetaObject *WidgetsPresenter::findWidgetMetaObject(const QMetaObject *viewModelMetaObject)
{
	auto currentMeta = viewModelMetaObject;
	while(currentMeta &&
		  currentMeta->inherits(&ViewModel::staticMetaObject) &&
		  currentMeta != &ViewModel::staticMetaObject) {
		if(d->explicitMappings.contains(currentMeta))
			return d->explicitMappings.value(currentMeta);
		else {
			QByteArray cName = currentMeta->className();
			//strip viewmodel
			auto lIndex = cName.lastIndexOf("ViewModel");
			if(lIndex > 0)
				cName.truncate(lIndex);
			//strip namespaces
			lIndex = cName.lastIndexOf("::");
			if(lIndex > 0)
				cName = cName.mid(lIndex + 2);

			auto shortest = std::numeric_limits<int>::max();
			const QMetaObject *res = nullptr;
			for(auto metaObject : qAsConst(d->implicitMappings)) {
				QByteArray vName = metaObject->className();
				//strip namespaces
				lIndex = vName.lastIndexOf("::");
				if(lIndex > 0)
					vName = vName.mid(lIndex + 2);

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
	auto tPresenter = dynamic_cast<IPresentingView*>(parentView);
	if(tPresenter && tPresenter->tryPresent(view))
		return true;

	auto metaObject = view->metaObject();

	// Check if QDialog
	if(metaObject->inherits(&QDialog::staticMetaObject)) {
		logDebug() << "Presenting" << metaObject->className() << "as dialog";
		qobject_cast<QDialog*>(view)->open();
		return true;
	}

	//Check if QMainWindow
	QWidget *central = nullptr;
	if(parentView && parentView->metaObject()->inherits(&QMainWindow::staticMetaObject)) {
		auto mainWindow = qobject_cast<QMainWindow*>(parentView);

		// Check if child is QDockWidget
		if(metaObject->inherits(&QDockWidget::staticMetaObject)) {
			logDebug() << "Presenting" << metaObject->className() << "as dock widget";
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

		// Check if QMdiArea and child ends with MdiWindow
		if(pView->metaObject()->inherits(&QMdiArea::staticMetaObject) &&
		   QByteArray(metaObject->className()).endsWith("MdiWindow")) {
			logDebug() << "Presenting" << metaObject->className() << "as MDI window";
			auto mdiArea = qobject_cast<QMdiArea*>(pView);
			mdiArea->addSubWindow(view);
			return true;
		}
	}

	//none of the special cases -> simply "show" it
	logDebug() << "Presenting" << metaObject->className() << "as normal window";
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
		qtHelp = config.viewProperties().value(QStringLiteral("showQtHelp"), qtHelp).toBool();
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
	auto props = config.viewProperties();
	info.parent = WidgetsPresenterPrivate::parent(props);
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
	msgBox->setAttribute(Qt::WA_DeleteOnClose);
	result->setCloseTarget(msgBox, QStringLiteral("reject()"));
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
	auto dialog = new QDialog{WidgetsPresenterPrivate::parent(config.viewProperties())};
	dialog->setAttribute(Qt::WA_DeleteOnClose);
	result->setCloseTarget(dialog, QStringLiteral("reject()"));
	auto layout = new QVBoxLayout(dialog);
	dialog->setLayout(layout);

	//set title and text
	auto text = config.text();
	if(text.isNull())
		text = config.title();
	else
		dialog->setWindowTitle(config.title());
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
		Q_ASSERT(btnBox->standardButtons().testFlag(sBtn)); //Must be the case now because of the change in MessageConfig
		btnBox->button(sBtn)->setText(it.value());
	}
	layout->addWidget(btnBox);

	//connect stuff
	connect(btnBox, &QDialogButtonBox::clicked,
			dialog, [dialog, btnBox](QAbstractButton *btn){
		dialog->done(btnBox->standardButton(btn));
	});
	connect(dialog, &QDialog::finished,
					 dialog, [input, property, result](int resCode){
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
	auto dialog = new QFileDialog{WidgetsPresenterPrivate::parent(props)};
	dialog->setAttribute(Qt::WA_DeleteOnClose);
	result->setCloseTarget(dialog, QStringLiteral("reject()"));

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
	connect(dialog, &QDialog::finished,
					 dialog, [dialog, isMultiFile, result](int resCode){
		if(result) {
			if(resCode == QDialog::Accepted) {
				if(isMultiFile)
					result->setResult(QVariant::fromValue(dialog->selectedUrls()));
				else if(!dialog->selectedUrls().isEmpty())
					result->setResult(dialog->selectedUrls().value(0));
				result->complete(MessageConfig::Ok);
			} else
				result->complete(MessageConfig::Cancel);
		}
	});

	//finalize and show
	dialog->adjustSize();
	DialogMaster::masterDialog(dialog);
	dialog->open();
}

void WidgetsPresenter::presentColorDialog(const MessageConfig &config, const QPointer<MessageResult> &result)
{
	auto props = config.viewProperties();
	auto dialog = new QColorDialog{WidgetsPresenterPrivate::parent(props)};
	dialog->setAttribute(Qt::WA_DeleteOnClose);
	result->setCloseTarget(dialog, QStringLiteral("reject()"));

	//prepare the dialog
	auto title = config.title();
	if(!title.isNull())
		dialog->setWindowTitle(title);
	auto color = config.defaultValue().value<QColor>();
	if(color.isValid())
		dialog->setCurrentColor(color);

	//set the color mode
	if(config.subType() == MessageConfig::SubTypeArgb)
		dialog->setOptions(QColorDialog::ShowAlphaChannel);
	else if(config.subType() == MessageConfig::SubTypeRgb)
		dialog->setOptions(nullptr);

	//set extra props
	for(auto it = props.constBegin(); it != props.constEnd(); it++)
		dialog->setProperty(qUtf8Printable(it.key()), it.value());

	//connect stuff
	connect(dialog, &QDialog::finished,
					 dialog, [dialog, result](int resCode){
		if(result) {
			if(resCode == QDialog::Accepted) {
				result->setResult(dialog->currentColor());
				result->complete(MessageConfig::Ok);
			} else
				result->complete(MessageConfig::Cancel);
		}
	});

	//finalize and show
	dialog->adjustSize();
	DialogMaster::masterDialog(dialog);
	dialog->open();
}

void WidgetsPresenter::presentProgressDialog(const MessageConfig &config, const QPointer<MessageResult> &result)
{
	auto control = config.defaultValue().value<ProgressControl*>();
	if(!control) {
		logWarning() << "ProgressControl was destroyed before a progress dialog could be shown";
		return;
	}
	auto props = config.viewProperties();
	auto dialog = new ProgressDialog{config, result, control, WidgetsPresenterPrivate::parent(props)};
	dialog->setAttribute(Qt::WA_DeleteOnClose);
	result->setCloseTarget(dialog, QStringLiteral("reject()"));
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
	implicitMappings{&SettingsDialog::staticMetaObject}
{}

WidgetsPresenter *WidgetsPresenterPrivate::currentPresenter()
{
	try {
#ifndef Q_NO_DEBUG
		Q_ASSERT_X(dynamic_cast<WidgetsPresenter*>(ServiceRegistry::instance()->service<IPresenter>()),
				   Q_FUNC_INFO,
				   "Cannot register widgets if the current presenter does not extend QtMvvm::WidgetsPresenter");
#endif
		return static_cast<WidgetsPresenter*>(ServiceRegistry::instance()->service<IPresenter>());
	} catch (QTMVVM_EXCEPTION_BASE &e) {
		qFatal("%s", e.what());
	}
}

QWidget *WidgetsPresenterPrivate::parent(const QVariantMap &properties)
{
	if(!properties.value(QStringLiteral("modal"), false).toBool())
		return QApplication::activeWindow();
	else
		return nullptr;
}

QValidator *QtMvvm::createUrlValidator(QStringList schemes, QObject *parent)
{
	return new QUrlValidator(std::move(schemes), parent);
}

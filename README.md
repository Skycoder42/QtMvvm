# QtMvvm
A mvvm oriented library for Qt, to create Projects for Widgets and Quick Controls 2 in parallel.

[![Travis Build Status](https://travis-ci.org/Skycoder42/QtMvvm.svg?branch=master)](https://travis-ci.org/Skycoder42/QtMvvm)
[![Appveyor Build status](https://ci.appveyor.com/api/projects/status/cwtt2n6af3dcbswa?svg=true)](https://ci.appveyor.com/project/Skycoder42/qtmvvm)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/367d7e7cd3d644939732c55c4c789a9d)](https://www.codacy.com/app/Skycoder42/QtMvvm)
[![AUR](https://img.shields.io/aur/version/qt5-mvvmcore.svg)](https://aur.archlinux.org/pkgbase/qt5-mvvm/)

![Demo Animation Widgets](./doc/images/sample_basic_widgets.gif)
![Demo Animation Quick](./doc/images/sample_basic_quick.gif)

For more images, check the [Images page](https://skycoder42.github.io/QtMvvm/images_page.html)

## Features
The main feature of QtMvvm is the seperation between ui and logic. With this library, you can create a core library, containing your application logic, as well as ui controllers (called "ViewModels"), and create multiple ui projects on top of it. This way you can for example provide both, a widgets and a qt quick based application, or create different uis for different devices, without having to code anything twice.

The key features are:

- Create ViewModels in the core application to prepare data for presentation without binding to any concret GUI
	- Supports singleton ViewModels
	- Supports automatic presentation of container ViewModels
- Functions to show messageboxes (info, warning, error, etc.) from your core app
	- Asynchronous, with result handling
	- Supports input dialogs and native file dialogs out of the box
		- Supports native file pickers on Android
	- Supports color picker dialog
	- Supports progress and busy indicator dialogs
	- custom dialog types can be created
- Methods to create Two-Way Bindings from C++ and QML
- Macros and a ServiceRegistry to make Dependency Injection possible for Services and ViewModels
- A generic Presenter Interface so you can create your own custom GUI implementations
	- Widgets and Qt Quick Controls 2 are supported out of the box
- Generic Edit-View factories to create simple edits for any kind of data from the core code
- Supports an extensive "Settings GUI" via a simple XML format

### QtMvvm Datasync
The QtMvvmDatasync modules help you to integrate [QtDataSync](https://github.com/Skycoder42/QtDataSync) (An easy and reliable synchronization library) into your projects. It adds ViewModels and Views to:

- Monitor and control the connection and synchronization status
- Manage your account and account devices
- Easy file based import and exports, as well as a fronted for the local Network-Exchange

### The Mvvm Pattern
If you don't know the Mvvm pattern already, you can read up on the links below. It's basically a clever seperation
of logic (the models), presentation logic (the viewmodels) and the actual GUI (the views) that is very useful when
creating applications that need to support different uis for the same data.

[![The MVVM Pattern](https://i-msdn.sec.s-msft.com/dynimg/IC564167.png)](https://msdn.microsoft.com/en-us/library/hh848246.aspx)

Good links to get started:

- https://en.wikipedia.org/wiki/Model%E2%80%93view%E2%80%93viewmodel
- https://msdn.microsoft.com/en-us/library/hh848246.aspx

## Download/Installation
1. Package Managers: The library is available via:
	- **Arch-Linux:** AUR-Repository: [`qt5-qtmvvm`](https://aur.archlinux.org/pkgbase/qt5-mvvm/)
	- **Ubuntu:** Launchpad-PPA: [ppa:skycoder42/qt-modules](https://launchpad.net/~skycoder42/+archive/ubuntu/qt-modules), package `libqt5mvvm[1/-dev]`
	- **MacOs:**
		- Tap: [`brew tap Skycoder42/qt-modules`](https://github.com/Skycoder42/homebrew-qt-modules)
		- Package: `qtmvvm`
		- **IMPORTANT:** Due to limitations of homebrew, you must run `source /usr/local/opt/qtmvvm/bashrc.sh` before you can use the module. Some goes for the `qtdatasync` dependency.
2. Simply add my repository to your Qt MaintenanceTool (Image-based How-To here: [Add custom repository](https://github.com/Skycoder42/QtModules/blob/master/README.md#add-my-repositories-to-qt-maintenancetool)):
	1. Start the MaintenanceTool from the commandline using `/path/to/MaintenanceTool --addTempRepository <url>` with one of the following urls (GUI-Method is currently broken, see [QTIFW-1156](https://bugreports.qt.io/browse/QTIFW-1156)) - This must be done *every time* you start the tool:
		- On Linux: https://install.skycoder42.de/qtmodules/linux_x64
		- On Windows: https://install.skycoder42.de/qtmodules/windows_x86
		- On Mac: https://install.skycoder42.de/qtmodules/mac_x64
	2. A new entry appears under all supported Qt Versions (e.g. `Qt > Qt 5.11 > Skycoder42 Qt modules`)
	3. You can install either all of my modules, or select the one you need: `Qt Mvvm`
	4. Continue the setup and thats it! you can now use the module for all of your installed Kits for that Qt Version
3. Download the compiled modules from the release page. **Note:** You will have to add the correct ones yourself and may need to adjust some paths to fit your installation! In addition to that, you will have to download the modules this one depends on as well. See Section "Requirements" below.
4. Build it yourself! **Note:** This requires all build an runtime dependencies to be available (See Section "Requirements" below). If you don't have/need cmake, you can ignore the related warnings. To automatically build and install to your Qt installation, run:
	- `qmake`
	- `make qmake_all`
	- `make` (If you want the tests/examples/etc. run `make all`)
	- Optional steps:
		- `make doxygen` to generate the documentation
		- `make lrelease` to generate the translations
	- `make install`

### Requirements
The library only has a few dependencies. The main modules only depends on qtbase and qtquick respectively. However, the Datasync extensions need QtDataSync of course.

- Custom Qt modules:
	- [QtDataSync](https://github.com/Skycoder42/QtDataSync) (DataSync extensions only)
- Additional stuff for building it yourself:
	- perl
	- [qpmx](https://github.com/Skycoder42/qpmx)
	- [qpm](https://github.com/Cutehacks/qpm)
	- Please note that you need the QtDataSync for building the datasync modules, but if you don't have it, the build wont fail. The datasync modules will simply be skipped.

### Modules
- **QtMvvmCore:** The core part of QtMvvm
- **QtMvvmWidgets:** The basic frontend for QtWidgets
- **QtMvvmQuick:** The basic frontend for QtQuick Controls 2
- **QtMvvmDataSyncCore:** The core part of the QtMvvm DataSync extension (requires QtDataSync)
- **QtMvvmDataSyncWidgets:** The frontend extensions of QtMvvm DataSync for QtWidgets (requires QtDataSync)
- **QtMvvmDataSyncQuick:** The frontend extensions of QtMvvm DataSync for QtQuick Controls 2 (requires QtDataSync)

## Usage
The following chapters will explain how to create a QtMvvm Project and how to correctly implement applications with it. A Mvvm Project always consists of one core project, with the application logic, and one or more gui projects with the View implementations. In the following section it is explained how to use QtMvvm without going into the depths. For more details you can check the sample projects. If you want to go deeper on how the Framework works and what detailed steps are needed, check out the [Documentation](https://skycoder42.github.io/QtMvvm/) of the following classes:

- CoreApp
- ViewModel
- ServiceRegistry
- IPresenter

The easiest way to create a QtMvvm Project is to use the provided project template. If you did not install via a package manager or the repository, follow the steps below to add the wizard.

### Add the custom wizard
If you did install the module as module you can skip this part. To create a new QtMvvm project, you can use a custom wizard for QtCreator. You will have to add it to your computer once. To do this, you will have to copy the contents of the [`ProjectTemplate`](ProjectTemplate) folder to a location known by QtCreator (Pro Tip: Use [Kinolien's Gitzip](https://kinolien.github.io/gitzip/) to download that directory only). The locations can be found here: [Locating Wizards](https://doc.qt.io/qtcreator/creator-project-wizards.html#locating-wizards). If you are, for example, working on linux, create a new folder called `QtMvvm` inside of `$HOME/.config/QtProject/qtcreator/templates/wizards` and copy the contents there. After restarting QtCreator, the project template should appear in the `Applications` section of the new-dialog as `QtMvvm Application Project`.

### Create and initialize the QtMvvm Project
Follow the setup to create the project. You can select the GUI-frontends you want to use, as well as additional features. After that you get a basic project skeleton with a simple CoreApp and a ViewModel, as well as the corresponding views.

For more Details on these classes, check the [Documentation](https://skycoder42.github.io/QtMvvm/).

**Important:** Due to [QTBUG-69963](https://bugreports.qt.io/browse/QTBUG-69963), you must always link to QtMvvmQuick when creating a QtQuick application for windows (See Issue #1). Since windows optimizes library and only links them if referenced at least once, simply add the line
```.cpp
QtMvvm::QuickPresenter::getInputViewFactory(); //Workaround for QTBUG-69963
```
to your `main.cpp` and it should work as expected

### Adding new ViewModels and Views
The most important part is to know how to add new ViewModels and Views.

#### Create the ViewModel
- Add a new c++ class to your core project. Let it inherit from `QtMvvm::ViewModel`
- Make sure the Constructor has the following signature: `Q_INVOKABLE MyClass(QObject *parent);`
- See [`examples/mvvmcore/SampleCore/sampleviewmodel.h`](examples/mvvmcore/SampleCore/sampleviewmodel.h) for an example ViewModel

#### Create the View for QtWidgets
- Create a new widget class in your widgets gui project.
- In order to use it as widget for a viewmodel, you should name it accordingly: If your viewmodel is named `MyCustomViewModel`, the widgets name must start with `MyCustom` as well (e.g. `MyCustomWindow`, `MyCustomDialog`, `MyCustomView`, etc.)
- Modify the constructor to look like this: `Q_INVOKABLE MyCustomWindow(QtMvvm::ViewModel *viewModel, QWidget *parent = nullptr);`
- Create a member variable with your viewmodel type, e.g. `MyCustomViewModel *_viewModel;`
- In the constructors implementation, cast the `viewModel` to your viewmodel class and assign it to `_viewModel`
- As final step, you need to register the widget. This can be done by adding the line `WidgetPresenter::registerView<MyCustomWindow>();` to your `main.cpp`, before calling `QApplication::exec`
- See [`examples/mvvmwidgets/SampleWidgets/sampleview.h`](examples/mvvmwidgets/SampleWidgets/sampleview.h) for an example widget

#### Create the quick ui
- *Optional:* Register the viewmodel for qml. This way autocomplete will work. Add the line `qmlRegisterUncreatableType<MyCustomViewModel>("com.example.mvvmexample", 1, 0, "MyCustomViewModel", "ViewModels cannot be created!");` to your main cpp before creating the engine.
- Create a new qml file inside of the folder `:/qml/views` (This is required to automatically find the view).
- In order to use it as view for a viewmodel, you should name it accordingly: If your viewmodel is named `MyCustomViewModel`, the views name must start with `MyCustom` as well (e.g. `MyCustomView`, `MyCustomActivity`, etc.)
- Add the following imports to the qml file:
```qml
import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1
import com.example.mvvmexample 1.0 //adjust to the module defined in your main.cpp
```
- Add a property named viewmodel to the root element: `property MyCustomViewModel viewmodel: null` (If you did not register the viewmodel, use `var` instead of `MyCustomViewModel` as property type)
- Add a presenter progress element to the root element. This way lazy loading views will show a progress: `PresenterProgress {}`
- See [`examples/mvvmquick/SampleQuick/SampleView.qml`](examples/mvvmquick/SampleQuick/SampleView.qml) for an example view

## Understanding how QtMvvm works
The general idea is the following: You create viewmodels in your core project, which represent uis. They typically contain all the properties relevant for the ui, methods (slots) that can be called (e.g. on a button click) and signals to inform the ui about changes and other events. Thus, you can use the as ui "placeholders". Of course, They only contain the ui logic, not the actual uis.

The CoreApp is what's reponsible for managing those viewmodels. Showing a viewmodel, as well as messages (alert dialogs) are all controlled by the coreapp. The coreapp uses a so called presenters to create the actual uis. The presenters are located in the ui projects and they are the most complicated part. Their main task is to find ui implementations for viewmodel (called views), and manage the life cycle as well as the presentation of those real views. The presenters are where the decision is made, how a specific view should be shown.

The views are whatever you need to create actual uis. This depends on the presenter used, since the presenter selects the views. Each ui type has their own way to create those views, but they all have in commmon that the views themselves do not control the application. When a viewmodel gets shown, a new view is created and the viewmodel passed to it. Once the views was closed, the view and the viewmodel get deleted again. Views and ViewModels are temporary and should only use the viewmodel to interact with other parts of the application.

### A side note on presenters
To create a presenter, the `QtMvvm::IPresenter` must be implemented and provided via the `ServiceRegistry`. Presenters can become quite complicated, but they are the thing you need to modify if you want to present views in a different way from the ones supported. Currently, the presenters can do the following:

- Widgets Presenter
	- Present viewmodels as widgets (or windows, dialogs, depending on type and parent)
	- Parent-Aware: If your viewmodel has a parent and shows a ui, the childs ui will be a child of the parents ui as well
	- Automatically detect QDockWidgets and place them as dock inside a parents QMainWindow
	- Automatically detect QMdiSubWindows and place them in a parents QMdiArea
	- Allows windows to implement `IPresentingWidget`. This way a window can handle the presentation of it's children without modifying the presenter
	- Allows to register custom input widgets for input dialogs (and other parts, like the settings. See `InputWidgetFactory`)
- Quick Presenter
	- Consists of a c++ part and a qml part
	- The c++ part is responsible for finding views for controls and creating them, but **not** for the actual presentation. This is done by the qml presenter
		- Allows to register custom input views for input dialogs (and other parts, like the settings. See `InputViewFactory`)
	- The qml presenter can be any qml type. It must provide the correct methods (as seen in `QtMvvmApp`) and register itself with `Component.onCompleted: QuickPresenter.qmlPresenter = self`
		- The `QtMvvmApp` qml types automatically register themselves as presenter and perform the presentations
		- Supports Items as new fullscreen pages inside a stack view, as drawer or as tabs
		- Supports Popups as modal dialogs

## Icons
In many of the UI projects default icons are used for the views (if no icon theme is present). They are taken from:

- Widgets: http://www.fatcow.com/free-icons
- Quick: https://material.io/icons/

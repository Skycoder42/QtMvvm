# QtMvvm
A mvvm oriented library for Qt, to create Projects for Widgets and Quick in parallel

**Note:** This project consists of many small github repositories. This repository is a meta-repository to keep documentation, examples, etc. in one place. Read the [Installation](#installation) chapter to learn how to use those repos.

## Features
The main feature of QtMvvm is the seperation between ui and logic. With this library, you can create a core library, containing your application logic, as well as ui controllers, and create multiple ui projects on top of it. This way you can for example provide both, a widgets and a qt quick based application, or create different uis for different devices. The key features are:

- The Control class to represent an ui element in your core app. Allows you to expose properties, signals and slots to the ui and control the application flow from your core app
- The IPresenter as interface to be implemented on each ui project. This is the part that presents uis based on controls
- The CoreApp to control the application startup independently of the QApplication used
- Functions to show messageboxes (info, warning, error, etc.) from your core app
	- Supports even input dialogs, with default edits as well as the option to add custom inputs

### QtMvvm Settings
Another feature is the QtMvvmSettings module. This extends the QtMvvm projects by adding ui independent settings. it is provided as seperate module to extend the basic QtMvvm.

- Create a control and an xml file (or custom formats) describing the settings ui in your core app
- Automatically creates a ui for widgets or quick based on the xml
	- Supports search as well as a reset functionality
	- Utilizes the same edits as the input dialogs
	
### QtMvvm Datasync
The QtMvvmDatasync module helps you to integrate [QtDataSync](https://github.com/Skycoder42/QtDataSync) (An easy and reliable synchronization library) into your projects. It adds uis to:

- Monitor the connection status
	- Current status
	- Synchronization progress
	- Displays errors (if present)
- Enable/disable synchronization
- Trigger synchronization
	- Including Resync
- Exchange User data (identity, encryption key, etc.)
	- Export/Import via files
	- UPD-based local network exchange

## Modules
- **QtMvvm** - The QtMvvm library itself
	- [`de.skycoder42.qtmvvm.core`](https://github.com/Skycoder42/QtMvvmCore) - The core part of qtmvvm
	- [`de.skycoder42.qtmvvm.widgets`](https://github.com/Skycoder42/QtMvvmWidgets) - The basic frontend for QtWidgets
	- [`de.skycoder42.qtmvvm.quick`](https://github.com/Skycoder42/QtMvvmQuick) - The basic frontend for QtQuick Controls 2
- **QtMvvmSettings** - A module adding an easy way to create settings dialogs
	- [`de.skycoder42.qtmvvm.settings.core`](https://github.com/Skycoder42/QtMvvmSettingsCore) - The core part of the settings module (the logic)
	- [`de.skycoder42.qtmvvm.settings.widgets`](https://github.com/Skycoder42/QtMvvmSettingsWidgets) - The ui implementation for QtWidgets
	- [`de.skycoder42.qtmvvm.settings.quick`](https://github.com/Skycoder42/QtMvvmSettingsQuick) - The ui implementation for QtQuick Controls 2
- **QtMvvmDatasync** - A module adding control and monitor uis for the QtDataSync library
	- [`de.skycoder42.qtmvvm.datasync.core`](https://github.com/Skycoder42/QtMvvmDatasyncCore) - The core part of the datasync module (the logic)
	- [`de.skycoder42.qtmvvm.datasync.widgets`](https://github.com/Skycoder42/QtMvvmDatasyncWidgets) - The ui implementation for QtWidgets
	- [`de.skycoder42.qtmvvm.datasync.quick`](https://github.com/Skycoder42/QtMvvmDatasyncQuick) - The ui implementation for QtQuick Controls 2
	
## Requirements
QtMvvm heavily relies on [qpm](https://www.qpm.io/). Check [GitHub - Installing](https://github.com/Cutehacks/qpm/blob/master/README.md#installing) to learn how to install qpm. You can of course use the project without qpm, but in that case you will have to collect all the dependencies by yourself. If you are unfamiliar with qpm, no worries, it's really easy to use.
	
## Gettings started
The following chapters will explain how to create a QtMvvm Project and how to correctly implement applications with it

### Add the custom wizard
Tho create a new QtMvvm project, you can use a custom wizard for QtCreator. You will have to add it to your computer once. To do this, you will have to copy the contents of the [`ProjectTemplate`](ProjectTemplate) folder to a location known by QtCreator (Pro Tip: Use [Kinolien's Gitzip](https://kinolien.github.io/gitzip/) to download that directory only). The locations can be found here: [Locating Wizards](https://doc.qt.io/qtcreator/creator-project-wizards.html#locating-wizards). If you are, for example, working on linux, create a new folder called `QtMvvm` inside of `$HOME/.config/QtProject/qtcreator/templates/wizards` and copy the contents there. After restarting QtCreator, the project template should appear in the `Applications` section of the new-dialog as `QtMvvm Application Project`.

### Create and initialize the QtMvvm Project
Follow the setup to create the project. You can select the GUI-frontends you want to use, as well as additional features. After the project has been created, qpm dependencies need to be installed (Sadly, this cannot be done by the wizard). To install them, simply run `qpm install` inside of every sub-project:

```sh
cd MyProjectCore
qpm install
cd ..

cd MyProjectWidgets
qpm install
cd ..

cd MyProjectQuick
qpm install
cd ..
```

Once that's done, the project should build and run successfully. (**Note:** Don't be irritated by the long build time. The qpm dependencies must be built initially, for all 3 projects. But since those don't change, only the first build/rebuilds take longer)

### Adding new controls
Finally, you may need to add custom controls. Currently, there is no custom wizard for that, but I may try to create one as well. Creating a new control can be done by the following steps:

#### Create the control
- Add a new c++ class to your core project. Let it inherit from `Control` and your done with the core part. You can show the control by creating one and calling `Control::show`
- See [`MvvmExample/MvvmExampleCore/maincontrol.h`](MvvmExample/MvvmExampleCore/maincontrol.h) for an example control

#### Create the widgets ui
- Create a new widget class in your widgets gui project.
- In order to use it as widget for a control, you should name it accordingly: If your control is named `MyCustomControl`, the widgets name must start with `MyCustom` as well (e.g. `MyCustomWindow`, `MyCustomDialog`, etc.)
- Modify the constructor to look like this: `Q_INVOKABLE MyCustomWindow(Control *mControl, QWidget *parent = nullptr);` (with the name adjusted to your class)
- Create a member variable with your control type, e.g. `MyCustomControl *control;`
- In the constructors implementation, cast the `mControl` to your control class and assign it to `control`
- As final step, you need to register the widget. This can be done by adding the line `WidgetPresenter::registerWidget<MyCustomWindow>();` to your `main.cpp`, before calling `QApplication::exec`
- See [`MvvmExample/MvvmExampleWidgets/mainwindow.h`](MvvmExample/MvvmExampleWidgets/mainwindow.h) for an example widget

#### Create the quick ui
- *Optional:* Register the control for qml. This way autocomplete will work. Add the line `qmlRegisterUncreatableType<MyCustomControl>("com.example.mvvmexample", 1, 0, "MyCustomControl", "Controls cannot be created!");` to your main cpp before creating the engine.
- Create a new qml file inside of the folder `:/qml/views` (This is required to automatically find the view).
- In order to use it as view for a control, you should name it accordingly: If your control is named `MyCustomControl`, the views name must start with `MyCustom` as well (e.g. `MyCustomView`, `MyCustomActivity`, etc.)
- Add the following imports to the qml file:
```qml
import QtQuick 2.8
import QtQuick.Controls 2.1
import de.skycoder42.quickextras 1.0
import de.skycoder42.qtmvvm.quick 1.0
import com.example.mvvmexample 1.0 //adjust to the module defined in your main.cpp
```
- Add a property named control to the root element: `property MyCustomControl control: null` (If you did not register the control, use `var` instead of `MyCustomControl` as property type)
- Add a presenter progress element to the root element. This way lazy loading views will show a progress: `PresenterProgress {}`
- See [`MvvmExample/MvvmExampleQuick/MainView.qml`](MvvmExample/MvvmExampleQuick/MainView.qml) for an example widget

## Understanding how QtMvvm works
The general idea is the following: You create controls in your core project, which represent uis. They typically contain all the properties relevant for the ui, methods (slots) that can be called (e.g. on a button click) and signals to inform the ui about changes and other events. The controls have show and close methods, just like widgets. Thus, you can use the as ui "placeholders". Of course, They only contain the ui logic, not the actual uis.

The CoreApp is what's reponsible for managing those controls. showing or closing a control, as well as messages (alert dialogs) are all controlled by the coreapp. The coreapp uses a so called presenter to create the actual uis. The presenters are located in the ui projects and they are the most complicated part. Their main task is to find ui implementations for controls, and manage the life cycle as well as the presentation of those real uis. The presenters are where the decision is made, how a specific ui should be shown.

The uis are whatever you need to create actual uis. This depends on the presenter used, since the presenter selects the uis. Each ui type has their own way to create those uis, but the all have in commmon, that the uis themselves do not control the application. When a control gets shown, a new ui is created and the control passed to it. Once the control was closed (via code or via the gui itself), the gui gets deleted again. Uis are temporary and should only use the control to interact with other
parts of the applications.

### A side note on presenters
To create a presenter, the `IPresenter` must be implemented. Presenters can become quite complicated, but they are the thing you need to modify if you want to present views in a different way from the ones supported. Currently, the presenters can do the following:

- Widgets Presenter (Presenter of `de.skycoder42.qtmvvm.widgets`)
	- Present controls as widgets (or windows, dialogs, depending on type and parent)
	- Parent-Aware: If your control has a parent and shows a ui, the childs ui will be a child of the parents ui as well
	- Automatically detect QDockWidgets and place them as dock inside a parents QMainWindow
	- Automatically detect QMdiSubWindows and place them in a parents QMdiArea
	- Allows windows to implement `IPresentingWidget`. This way a window can handle the presentation of it's children without modifying the presenter
	- Allows to register custom input widgets for input dialogs (and other parts, like the settings. See `InputWidgetFactory`)
- Quick Presenter (Presenter of `de.skycoder42.qtmvvm.quick`)
	- Consists of a c++ part and a qml part
	- The c++ part is responsible for finding views for controls and creating them, but **not** for the actual presentation. This is done by the qml presenter
		- Allows to register custom input views for input dialogs (and other parts, like the settings. See `InputViewFactory`)
	- The qml presenter can be any qml type. It must provide the correct methods (as seen in `AppBase`) and register itself with `QuickPresenter.qmlPresenter = root`
		- The `AppBase` or `App` qml types automatically register themselves as presenter and perform the presentations
		- Supports Items as new fullscreen pages inside a stack view 
		- Supports Popups as modal dialogs

## Installation
All those modules are available as qpm packages. To install any of them:

1. Install qpm (See [GitHub - Installing](https://github.com/Cutehacks/qpm/blob/master/README.md#installing), for **windows** see below)
2. In your projects root directory, run `qpm install de.skycoder42.qtmvvm...` (The package you need)
3. Include qpm to your project by adding `include(vendor/vendor.pri)` to your `.pro` file

Check their [GitHub - Usage for App Developers](https://github.com/Cutehacks/qpm/blob/master/README.md#usage-for-app-developers) to learn more about qpm.

**Important for Windows users:** QPM Version *0.10.0* (the one you can download on the website) is currently broken on windows! It's already fixed in master, but not released yet. Until a newer versions gets released, you can download the latest dev build from here:
- https://storage.googleapis.com/www.qpm.io/download/latest/windows_amd64/qpm.exe
- https://storage.googleapis.com/www.qpm.io/download/latest/windows_386/qpm.exe

## Icons
In many of the UI projects default icons are used for the views (if no icon theme is present). They are taken from:

- Widgets: http://www.fatcow.com/free-icons
- Quick: https://material.io/icons/

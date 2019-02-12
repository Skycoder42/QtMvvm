QT += core qml quick svg quickcontrols2 mvvmquick mvvmcore-private mvvmquick-private
CXX_MODULE = mvvmquick
TARGETPATH = de/skycoder42/QtMvvm/Quick
TARGET  = declarative_mvvmquick
IMPORT_VERSION = $$MODULE_VERSION_IMPORT
DEFINES += "VERSION_MAJOR=$$MODULE_VERSION_MAJOR"
DEFINES += "VERSION_MINOR=$$MODULE_VERSION_MINOR"

HEADERS += \
	qtmvvmquick_plugin.h \
	qqmlquickpresenter.h \
	settingsuibuilder.h \
	settingssectionmodel.h \
	multifilterproxymodel.h \
	settingsentrymodel.h \
	qqmlviewplaceholder.h

SOURCES += \
	qtmvvmquick_plugin.cpp \
	qqmlquickpresenter.cpp \
	settingsuibuilder.cpp \
	settingssectionmodel.cpp \
	multifilterproxymodel.cpp \
	settingsentrymodel.cpp \
	qqmlviewplaceholder.cpp

QML_FILES += \
	QtMvvmApp.qml \
	QtMvvmApp10.qml \
	PresentingStackView.qml \
	PresentingStackView10.qml \
	PresenterProgress.qml \
	PopupPresenter.qml \
	PopupPresenter10.qml \
	DialogPresenter.qml \
	DialogPresenter10.qml \
	TintIcon.qml \
	AlertDialog.qml \
	ContrastToolBar.qml \
	ToolBarLabel.qml \
	ActionButton.qml \
	RoundActionButton.qml \
	MenuButton.qml \
	MsgBoxBase.qml \
	MsgBox.qml \
	InputDialog.qml \
	PresentingDrawer.qml \
	ListSection.qml \
	SectionListView.qml \
	OverviewListView.qml \
	SettingsView.qml \
	SearchBar.qml \
	RoundMenuButton.qml \
	DecorLabel.qml \
	MsgDelegate.qml \
	TimeTumbler.qml \
	TimeEdit.qml \
	DateEdit.qml \
	ColorHelper.qml \
	ColorEdit.qml \
	ProgressDialog.qml

RESOURCES += \
	qtmvvmquick_plugin.qrc

OTHER_FILES += qmldir

android {
	QT += androidextras
	HEADERS += androidfilechooser.h
	SOURCES += androidfilechooser.cpp
}

CONFIG += qmlcache
load(qml_plugin)

generate_qmltypes {
	# run again to overwrite module env
	ldpath.name = LD_LIBRARY_PATH
	ldpath.value = "$$shadowed($$dirname(_QMAKE_CONF_))/lib/:$$[QT_INSTALL_LIBS]:$$(LD_LIBRARY_PATH)"
	qmlpath.name = QML2_IMPORT_PATH
	qmlpath.value = "$$shadowed($$dirname(_QMAKE_CONF_))/qml/:$$[QT_INSTALL_QML]:$$(QML2_IMPORT_PATH)"
	PLGDUMP_ENV = ldpath qmlpath
	QT_TOOL_ENV = ldpath qmlpath
	qtPrepareTool(QMLPLUGINDUMP, qmlplugindump)
	QT_TOOL_ENV =

	#overwrite the target deps as make target is otherwise not detected
	qmltypes.depends = ../../../qml/$$TARGETPATH/$(TARGET)
	OLDDMP = $$take_first(qmltypes.commands)
	qmltypes.commands = $$QMLPLUGINDUMP $${qmltypes.commands}
	message("replaced $$OLDDMP with $$QMLPLUGINDUMP")

	mfirst.target = all
	mfirst.depends += qmltypes
	QMAKE_EXTRA_TARGETS += mfirst
}

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
	QtMvvmApp11.qml \
	PresentingStackView.qml \
	PresentingStackView11.qml \
	PresenterProgress.qml \
	PopupPresenter.qml \
	PopupPresenter11.qml \
	DialogPresenter.qml \
	DialogPresenter11.qml \
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
	SettingsView11.qml \
	SearchBar.qml \
	RoundMenuButton.qml \
	DecorLabel.qml \
	MsgDelegate.qml \
	TimeEdit.qml

RESOURCES += \
	qtmvvmquick_plugin.qrc

OTHER_FILES += qmldir

android {
	QT += androidextras
	HEADERS += androidfilechooser.h
	SOURCES += androidfilechooser.cpp
}

generate_qmltypes {
	typeextra1.target = qmltypes
	typeextra1.depends += export LD_LIBRARY_PATH := "$$shadowed($$dirname(_QMAKE_CONF_))/lib/:$$[QT_INSTALL_LIBS]:$(LD_LIBRARY_PATH)"
	typeextra2.target = qmltypes
	typeextra2.depends += export QML2_IMPORT_PATH := "$$shadowed($$dirname(_QMAKE_CONF_))/qml/"
	QMAKE_EXTRA_TARGETS += typeextra1 typeextra2
}

CONFIG += qmlcache
load(qml_plugin)

generate_qmltypes {
	qmltypes.depends = ../../../qml/$$TARGETPATH/$(TARGET)  #overwrite the target deps
	qmltypes.commands += "2>/dev/null"  # kill invalid qml warnings

	mfirst.target = all
	mfirst.depends += qmltypes
	QMAKE_EXTRA_TARGETS += mfirst
}

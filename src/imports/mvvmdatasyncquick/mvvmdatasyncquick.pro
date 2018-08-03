QT += core qml quick svg mvvmdatasyncquick
CXX_MODULE = mvvmdatasyncquick
TARGETPATH = de/skycoder42/QtMvvm/DataSync/Quick
TARGET  = declarative_mvvmdatasyncquick
IMPORT_VERSION = $$MODULE_VERSION_IMPORT
DEFINES += "VERSION_MAJOR=$$MODULE_VERSION_MAJOR"
DEFINES += "VERSION_MINOR=$$MODULE_VERSION_MINOR"

HEADERS += \
	qtmvvmdatasyncquick_plugin.h

SOURCES += \
	qtmvvmdatasyncquick_plugin.cpp

QML_FILES += \
	DataSyncView.qml \
	NetworkExchangeView.qml \
	IdentityEditView.qml \
	ExportSetupView.qml \
	ChangeRemoteView.qml

RESOURCES += \
	qtmvvmdatasyncquick_plugin.qrc

OTHER_FILES += qmldir

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

QT += core qml quick mvvmdatasynccore
CXX_MODULE = mvvmdatasynccore
TARGETPATH = de/skycoder42/QtMvvm/DataSync/Core
TARGET  = declarative_mvvmdatasynccore
IMPORT_VERSION = $$MODULE_VERSION_IMPORT
DEFINES += "VERSION_MAJOR=$$MODULE_VERSION_MAJOR"
DEFINES += "VERSION_MINOR=$$MODULE_VERSION_MINOR"

HEADERS += \
	qtmvvmdatasynccore_plugin.h

SOURCES += \
	qtmvvmdatasynccore_plugin.cpp

OTHER_FILES += qmldir

generate_qmltypes {
	typeextra1.target = qmltypes
	typeextra1.depends += export LD_LIBRARY_PATH := "$$shadowed($$dirname(_QMAKE_CONF_))/lib/:$(LD_LIBRARY_PATH)"
	typeextra2.target = qmltypes
	typeextra2.depends += export QML2_IMPORT_PATH := "$$shadowed($$dirname(_QMAKE_CONF_))/qml/"
	QMAKE_EXTRA_TARGETS += typeextra1 typeextra2
}

CONFIG += qmlcache
load(qml_plugin)

generate_qmltypes {
	qmltypes.depends = ../../../qml/$$TARGETPATH/$(TARGET)  #overwrite the target deps

	mfirst.target = all
	mfirst.depends += qmltypes
	QMAKE_EXTRA_TARGETS += mfirst
}

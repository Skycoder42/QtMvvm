QT += core qml quick mvvmcore
CXX_MODULE = mvvmcore
TARGETPATH = de/skycoder42/QtMvvm/Core
TARGET  = declarative_mvvmcore
IMPORT_VERSION = $$MODULE_VERSION_IMPORT
DEFINES += "VERSION_MAJOR=$$MODULE_VERSION_MAJOR"
DEFINES += "VERSION_MINOR=$$MODULE_VERSION_MINOR"

HEADERS += \
	qtmvvmcore_plugin.h \
	qqmlmvvmbinding.h \
	qqmlmvvmmessage.h \
	qqmlserviceregistry.h \
	qqmlcoreapp.h

SOURCES += \
	qtmvvmcore_plugin.cpp \
	qqmlmvvmbinding.cpp \
	qqmlmvvmmessage.cpp \
	qqmlserviceregistry.cpp \
	qqmlcoreapp.cpp

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

	mfirst.target = all
	mfirst.depends += qmltypes
	QMAKE_EXTRA_TARGETS += mfirst
}

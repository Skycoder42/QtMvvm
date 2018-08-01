TEMPLATE = lib
CONFIG += plugin

QT = core mvvmcore

load(qt_build_paths)

TARGET = $$qtLibraryTarget(serviceregistry_testplugin)
DESTDIR = $$MODULE_BASE_OUTDIR/tests/plugins/serviceregistry

HEADERS += \
	testplugin.h \
	../serviceregistry/plugintestinterface.h

SOURCES += \
	testplugin.cpp

DISTFILES += serviceregistrytestplugin.json

QMAKE_EXTRA_TARGETS += run-tests

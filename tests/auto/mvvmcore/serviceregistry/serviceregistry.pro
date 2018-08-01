TEMPLATE = app

QT += testlib mvvmcore
QT -= gui
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_serviceregistry

HEADERS += \
	testinterface.h \
	testservice.h \
	testobject.h \
	plugintestinterface.h

SOURCES += \
	tst_serviceregistry.cpp \
	testservice.cpp \
	testobject.cpp

load(qt_build_paths)
DEFINES += PLUGIN_TESTDIR=\\\"$$MODULE_BASE_OUTDIR/tests/plugins\\\"

include(../../testrun.pri)

TEMPLATE = app

QT += testlib mvvmdatasynccore
QT -= gui
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_datasyncsettingsaccessor

HEADERS += \
	../../../shared/tst_isettingsaccessor.h

SOURCES += \
	tst_datasyncsettingsaccessor.cpp

include(../../testrun.pri)

TEMPLATE = app

QT += testlib mvvmcore
QT -= gui
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_qsettingsaccessor

HEADERS += \
	../../../shared/tst_isettingsaccessor.h

SOURCES += \
	tst_qsettingsaccessor.cpp

include(../../testrun.pri)

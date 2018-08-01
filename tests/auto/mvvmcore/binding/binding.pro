TEMPLATE = app

QT += testlib mvvmcore
QT -= gui
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_binding

SOURCES += \
	tst_binding.cpp

include(../../testrun.pri)

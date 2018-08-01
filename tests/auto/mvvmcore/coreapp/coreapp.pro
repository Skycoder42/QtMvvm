TEMPLATE = app

QT += testlib mvvmcore
QT -= gui
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_coreapp

SOURCES += \
	tst_coreapp.cpp \
	testapp.cpp \
	testpresenter.cpp \
	testviewmodel.cpp

HEADERS += \
	testapp.h \
	testpresenter.h \
	testviewmodel.h

include(../../testrun.pri)

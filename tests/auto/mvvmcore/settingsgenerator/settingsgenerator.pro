TEMPLATE = app

QT += testlib mvvmcore
QT -= gui
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_settingsgenerator

SOURCES += \
		tst_settingsgenerator.cpp \
    testbackend.cpp

SETTINGS_DEFINITIONS += \
	settings.xml

HEADERS += \
    testbackend.h

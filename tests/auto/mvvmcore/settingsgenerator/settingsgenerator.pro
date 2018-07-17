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
	generatortest.xml \
	importtest.xml

HEADERS += \
	testbackend.h

DISTFILES += \
	import_normal.xml \
	import_config.xml

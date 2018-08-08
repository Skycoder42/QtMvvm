TEMPLATE = app

QT += testlib mvvmcore
QT -= gui
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_settingsgenerator

HEADERS += \
	testbackend.h

SOURCES += \
	tst_settingsgenerator.cpp \
	testbackend.cpp

SETTINGS_DEFINITIONS += \
	generatortest.xml \
	importtest.xml

# CONFIG += no_auto_lupdate
SETTINGS_TRANSLATIONS += \
	translatortest.xml

DISTFILES += \
	import_normal.xml \
	import_config.xml

TRANSLATIONS += test_de.ts

include(../../testrun.pri)

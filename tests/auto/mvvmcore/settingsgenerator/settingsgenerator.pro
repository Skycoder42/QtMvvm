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

SETTINGS_TRANSLATIONS += \
	translatortest.xml

#_never_true: SOURCES += $$files(.ts-dummy/*)

DISTFILES += \
	import_normal.xml \
	import_config.xml

TRANSLATIONS += test_de.ts

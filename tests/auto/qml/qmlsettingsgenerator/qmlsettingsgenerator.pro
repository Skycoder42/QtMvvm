TEMPLATE = app

QT += testlib mvvmcore qml
CONFIG += qmltestcase console
CONFIG -= app_bundle

TARGET = tst_qmlsettingsgenerator

HEADERS += \
	testbackend.h

SOURCES += \
	tst_qmlsettingsgenerator.cpp \
	testbackend.cpp

QML_SETTINGS_DEFINITIONS += \
	generatortest.xml

SETTINGS_DEFINITIONS += $$QML_SETTINGS_DEFINITIONS

DISTFILES += \
	tst_qmlsettingsgenerator.qml

importFiles.path = .
DEPLOYMENT += importFiles

win32:msvc:CONFIG(debug, debug|release): CONFIG += disable_testrun
include(../../testrun.pri)

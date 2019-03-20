TEMPLATE = app

QT += testlib mvvmcore qml
CONFIG += qmltestcase console
CONFIG -= app_bundle

TARGET = tst_qmlmvvmcore

SOURCES += \
	tst_qmlmvvmcore.cpp

DISTFILES += \
	tst_qmlmvvmcore.qml

importFiles.path = .
DEPLOYMENT += importFiles

win32:msvc:CONFIG(debug, debug|release): CONFIG += disable_testrun
include(../../testrun.pri)

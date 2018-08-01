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

include(../../testrun.pri)

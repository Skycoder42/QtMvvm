TEMPLATE = app

QT += testlib mvvmcore qml
CONFIG += qmltestcase console
CONFIG -= app_bundle

TARGET = tst_qmlsettingsgenerator

SOURCES += \
	tst_qmlsettingsgenerator.cpp

QML_SETTINGS_DEFINITIONS += \
	generatortest.xml

SETTINGS_DEFINITIONS += $$QML_SETTINGS_DEFINITIONS

DISTFILES += \
	tst_qmlsettingsgenerator.qml

importFiles.path = .
DEPLOYMENT += importFiles

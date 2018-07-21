TEMPLATE = app

QT += testlib mvvmcore qml
CONFIG += qmltestcase console
CONFIG -= app_bundle

TARGET = tst_qmlsettingsgenerator

SOURCES += \
	tst_qmlsettingsgenerator.cpp

QML_SETTINGS_DEFINITIONS += \
	../../mvvmcore/settingsgenerator/generatortest.xml

DISTFILES += \
	tst_qmlsettingsgenerator.qml

importFiles.path = .
DEPLOYMENT += importFiles

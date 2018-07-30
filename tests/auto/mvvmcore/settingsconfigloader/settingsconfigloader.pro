TEMPLATE = app

QT += testlib mvvmcore mvvmcore-private
QT -= gui
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_settingsconfigloader

DEFINES += SRCDIR=\\\"$$_PRO_FILE_PWD_\\\"

INCLUDEPATH += \
	../../../../src/settingsconfig \
	$$shadowed(../../../../src/mvvmcore) \
	../../../../src/3rdparty/optional-lite \
	../../../../src/3rdparty/variant-lite

SOURCES += \
	tst_settingsconfigloader.cpp

DISTFILES += \
    entryDocument.xml \
    groupDocument.xml \
    sectionDocument.xml \
    categoryDocument.xml \
    selectorDocument.xml

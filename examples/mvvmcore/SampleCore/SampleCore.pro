TEMPLATE = lib

QT = core gui mvvmcore
CONFIG += static

TARGET = SampleCore

HEADERS += \
	samplecoreapp.h \
	sampleviewmodel.h \
	ieventservice.h \
	echoservice.h \
	resultviewmodel.h \
	drawerviewmodel.h \
	tabviewmodel.h

SOURCES += \
	samplecoreapp.cpp \
	sampleviewmodel.cpp \
	echoservice.cpp \
	resultviewmodel.cpp \
	drawerviewmodel.cpp \
	tabviewmodel.cpp

RESOURCES += \
	sample_core.qrc

QTMVVM_TS_SETTINGS += settings.xml
_never_true_condition: SOURCES += $$files($$PWD/.ts-dummy/*) #TODO document
PRE_TARGETDEPS += qtmvvm-tsgen

TRANSLATIONS += samplecore_de.ts
DISTFILES += $$TRANSLATIONS

target.path = $$[QT_INSTALL_EXAMPLES]/mvvmcore/$$TARGET
INSTALLS += target

samples_in_build: QMAKE_QSETTINGSTRANSLATOR = $$PWD/../../../bin/qsettingstranslator.py

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
	tabviewmodel.h \
	containerviewmodel.h

SOURCES += \
	samplecoreapp.cpp \
	sampleviewmodel.cpp \
	echoservice.cpp \
	resultviewmodel.cpp \
	drawerviewmodel.cpp \
	tabviewmodel.cpp \
	containerviewmodel.cpp

RESOURCES += \
	sample_core.qrc

SETTINGS_TRANSLATIONS += settings.xml

TRANSLATIONS += samplecore_de.ts
DISTFILES += $$TRANSLATIONS

target.path = $$[QT_INSTALL_EXAMPLES]/mvvmcore/$$TARGET
INSTALLS += target

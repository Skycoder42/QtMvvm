TEMPLATE = lib

QT = core gui mvvmcore mvvmsettingscore
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

target.path = $$[QT_INSTALL_EXAMPLES]/mvvmcore/$$TARGET
INSTALLS += target

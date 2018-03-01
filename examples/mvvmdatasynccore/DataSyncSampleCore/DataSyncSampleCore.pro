TEMPLATE = lib

QT = core gui mvvmcore mvvmdatasynccore
CONFIG += static

TARGET = DataSyncSampleCore

HEADERS += \
	samplecoreapp.h \
	sampleviewmodel.h \
	sampledata.h

SOURCES += \
	samplecoreapp.cpp \
	sampleviewmodel.cpp \
	sampledata.cpp

OTHER_FILES += docker-compose.yaml

target.path = $$[QT_INSTALL_EXAMPLES]/mvvmdatasynccore/$$TARGET
INSTALLS += target

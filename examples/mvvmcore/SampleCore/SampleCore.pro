TEMPLATE = lib

QT = core gui mvvmcore

TARGET = SampleCore

DEFINES += SAMPLECORE_LIBRARY

HEADERS += \
		samplecore_global.h \
	samplecoreapp.h \
    sampleviewmodel.h \
    ieventservice.h \
    echoservice.h

SOURCES += \
	samplecoreapp.cpp \
    sampleviewmodel.cpp \
    echoservice.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/mvvmcore/$$TARGET
INSTALLS += target

#not found by linker?
unix:!mac {
	LIBS += -L$$OUT_PWD/../../../lib #required to make this the first place to search
	LIBS += -L$$[QT_INSTALL_LIBS] -licudata
	LIBS += -L$$[QT_INSTALL_LIBS] -licui18n
	LIBS += -L$$[QT_INSTALL_LIBS] -licuuc
}

#add lib dir to rpath
mac: QMAKE_LFLAGS += '-Wl,-rpath,\'$$OUT_PWD/../../../lib\''

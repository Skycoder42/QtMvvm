TEMPLATE = app

QT  += core gui qml quick mvvmquick widgets quickcontrols2

TARGET = SampleQuick

HEADERS += \
	quickeventservice.h

SOURCES += main.cpp \
	quickeventservice.cpp

RESOURCES += qml.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/mvvmquick/$$TARGET
INSTALLS += target

#not found by linker?
linux:!android {
	LIBS += -L$$OUT_PWD/../../../lib #required to make this the first place to search
	LIBS += -L$$[QT_INSTALL_LIBS] -licudata
	LIBS += -L$$[QT_INSTALL_LIBS] -licui18n
	LIBS += -L$$[QT_INSTALL_LIBS] -licuuc
}

#add lib dir to rpath
mac: QMAKE_LFLAGS += '-Wl,-rpath,\'$$OUT_PWD/../../../lib\''

#link to core lib
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../mvvmcore/SampleCore/release/ -lSampleCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../mvvmcore/SampleCore/debug/ -lSampleCore
else:unix: LIBS += -L$$OUT_PWD/../../mvvmcore/SampleCore/ -lSampleCore

INCLUDEPATH += $$PWD/../../mvvmcore/SampleCore
DEPENDPATH += $$PWD/../../mvvmcore/SampleCore

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../mvvmcore/SampleCore/release/libSampleCore.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../mvvmcore/SampleCore/debug/libSampleCore.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../mvvmcore/SampleCore/release/SampleCore.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../mvvmcore/SampleCore/debug/SampleCore.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../mvvmcore/SampleCore/libSampleCore.a

# thanks to qmlc files, only the import path must be fixed
samples_in_build {
	QML_IMPORT_PATH = $$OUT_PWD/../../../qml
	DEFINES += QML_PATH=\\\"$$QML_IMPORT_PATH\\\"
}

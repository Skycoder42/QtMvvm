TEMPLATE = app

QT  += core gui widgets mvvmwidgets

TARGET = SampleWidgets

HEADERS += \
	widgetseventservice.h \
	sampleview.h \
	resultdialog.h \
	tabview.h \
	tabitemview.h

SOURCES += \
		main.cpp \
	widgetseventservice.cpp \
	sampleview.cpp \
	resultdialog.cpp \
	tabview.cpp \
	tabitemview.cpp

FORMS += \
	sampleview.ui \
	resultdialog.ui \
	tabview.ui \
	tabitemview.ui

target.path = $$[QT_INSTALL_EXAMPLES]/mvvmwidgets/$$TARGET
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

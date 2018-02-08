TEMPLATE = app

QT  += core gui widgets mvvmwidgets

TARGET = SampleWidgets

HEADERS += \
		mainwindow.h

SOURCES += \
		main.cpp \
		mainwindow.cpp

FORMS += \
		mainwindow.ui

target.path = $$[QT_INSTALL_EXAMPLES]/mvvmwidgets/$$TARGET
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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../mvvmcore/SampleCore/release/ -lSampleCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../mvvmcore/SampleCore/debug/ -lSampleCore
else:unix: LIBS += -L$$OUT_PWD/../../mvvmcore/SampleCore/ -lSampleCore

INCLUDEPATH += $$PWD/../../mvvmcore/SampleCore
DEPENDPATH += $$PWD/../../mvvmcore/SampleCore

TEMPLATE = app

QT += quick mvvmcore

TARGET = DroidSettings

SOURCES += \
	main.cpp \
    settings.cpp

RESOURCES += qml.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/mvvmcore/$$TARGET
INSTALLS += target

HEADERS += \
    settings.h

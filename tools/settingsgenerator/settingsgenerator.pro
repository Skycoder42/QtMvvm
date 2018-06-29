option(host_build)

QT = core

TARGET = qsettingsgenerator
VERSION = $$MODULE_VERSION
COMPANY = Skycoder42
BUNDLE_PREFIX = de.skycoder42

DEFINES += BUILD_QSETTINGSGENERATOR
DEFINES += "TARGET=\\\"$$TARGET\\\""
DEFINES += "VERSION=\\\"$$VERSION\\\""
DEFINES += "COMPANY=\\\"$$COMPANY\\\""
DEFINES += "BUNDLE_PREFIX=\\\"$$BUNDLE_PREFIX\\\""

HEADERS +=

SOURCES += \
	main.cpp

load(qt_tool)

win32 {
	QMAKE_TARGET_PRODUCT = "Qt Settings Builder"
	QMAKE_TARGET_COMPANY = $$COMPANY
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = $${BUNDLE_PREFIX}.
}

DISTFILES += qsettingsgenerator.xsd

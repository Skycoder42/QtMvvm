option(host_build)

QT = core
!force_bootstrap:qtHaveModule(xmlpatterns): QT += xmlpatterns

TARGET = qsettingsgenerator
VERSION = $$MODULE_VERSION
COMPANY = Skycoder42
BUNDLE_PREFIX = de.skycoder42

DEFINES += BUILD_QSETTINGSGENERATOR
DEFINES += "TARGET=\\\"$$TARGET\\\""
DEFINES += "VERSION=\\\"$$VERSION\\\""
DEFINES += "COMPANY=\\\"$$COMPANY\\\""
DEFINES += "BUNDLE_PREFIX=\\\"$$BUNDLE_PREFIX\\\""

HEADERS += \
	settingstranslator.h \
    cppsettingsgenerator.h \
    settingsgeneratorimpl.h \
    qmlsettingsgenerator.h

SOURCES += \
	main.cpp \
	settingstranslator.cpp \
    cppsettingsgenerator.cpp \
    settingsgeneratorimpl.cpp \
    qmlsettingsgenerator.cpp

XML_SCHEMA_DEFINITIONS += \
	qsettingsgenerator.xsd

include(../../src/settingsconfig/settingsconfig.pri)

contains(QT, xmlpatterns):RESOURCES += qsettingsgenerator.qrc

load(qt_tool)

win32 {
	QMAKE_TARGET_PRODUCT = "Qt Settings Builder"
	QMAKE_TARGET_COMPANY = $$COMPANY
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = $${BUNDLE_PREFIX}.
}

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)

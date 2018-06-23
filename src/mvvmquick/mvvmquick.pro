TARGET = QtMvvmQuick

QT = core gui qml quick svg mvvmcore mvvmcore-private

HEADERS += \
	qtmvvmquick_global.h \
	quickpresenter.h \
	quickpresenter_p.h \
	inputviewfactory.h \
	inputviewfactory_p.h \
    formatters_p.h

SOURCES += \
	quickpresenter.cpp \
	inputviewfactory.cpp

RESOURCES += \
	qtmvvmquick_module.qrc

TRANSLATIONS += \
	translations/qtmvvmquick_de.ts \
	translations/qtmvvmquick_template.ts

DISTFILES += $$TRANSLATIONS

qpmx_ts_target.path = $$[QT_INSTALL_TRANSLATIONS]
qpmx_ts_target.depends += lrelease
INSTALLS += qpmx_ts_target

load(qt_module)

win32 {
	QMAKE_TARGET_PRODUCT = "$$TARGET"
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "com.skycoder42."
}

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)

qpmx_ts_target.files -= $$OUT_PWD/$$QPMX_WORKINGDIR/qtmvvmquick_template.qm
qpmx_ts_target.files += translations/qtmvvmquick_template.ts

mingw: LIBS_PRIVATE += -lQt5Gui -lQt5Core

# source include for lupdate
never_true_for_lupdate {
	SOURCES += $$files(../imports/mvvmquick/*.cpp) \
		$$files(../imports/mvvmquick/*.qml)
}

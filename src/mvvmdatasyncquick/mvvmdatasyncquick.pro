TARGET = QtMvvmDataSyncQuick

QT = core gui quick mvvmdatasynccore mvvmquick mvvmdatasynccore-private

HEADERS += \
	qtmvvmdatasyncquick_global.h

SOURCES += \
    qtmvvmdatasyncquick_global.cpp

TRANSLATIONS += \
	translations/qtmvvmdatasyncquick_de.ts \
	translations/qtmvvmdatasyncquick_template.ts

DISTFILES += $$TRANSLATIONS

qpmx_ts_target.path = $$[QT_INSTALL_TRANSLATIONS]
qpmx_ts_target.depends += lrelease

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

qpmx_ts_target.files -= $$OUT_PWD/$$QPMX_WORKINGDIR/qtmvvmdatasyncquick_template.qm
qpmx_ts_target.files += translations/qtmvvmdatasyncquick_template.ts

RESOURCES += \
    qtmvvmdatasyncquick_module.qrc


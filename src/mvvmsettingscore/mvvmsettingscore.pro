TARGET = QtMvvmSettingsCore

QT = core gui mvvmcore mvvmcore-private

HEADERS += qtmvvmsettingscore_global.h \
	settingsviewmodel.h \
	settingssetup.h \
	settingssetuploader_p.h \
    settingsviewmodel_p.h

SOURCES += \
	settingsviewmodel.cpp \
	settingssetuploader.cpp \
    qtmvvmsettingscore_global.cpp

TRANSLATIONS += \
	translations/qtmvvsettingsmcore_de.ts \
	translations/qtmvvsettingsmcore_template.ts

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

qpmx_ts_target.files -= $$OUT_PWD/$$QPMX_WORKINGDIR/qtmvvsettingsmcore_template.qm
qpmx_ts_target.files += translations/qtmvvsettingsmcore_template.ts


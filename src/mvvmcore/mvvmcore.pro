TARGET = QtMvvmCore

QT = core gui

HEADERS += \
	viewmodel.h \
	qtmvvmcore_global.h \
	viewmodel_p.h \
	coreapp.h \
	coreapp_p.h

SOURCES += \
	viewmodel.cpp \
	coreapp.cpp

TRANSLATIONS += \
	translations/qtmvvmcore_de.ts \
	translations/qtmvvmcore_template.ts

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

qpmx_ts_target.files -= $$OUT_PWD/$$QPMX_WORKINGDIR/qtmvvmcore_template.qm
qpmx_ts_target.files += translations/qtmvvmcore_template.ts

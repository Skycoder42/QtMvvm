TARGET = QtMvvmWidgets

QT = core gui widgets mvvmcore mvvmcore-private

HEADERS += \
	qtmvvmwidgets_global.h \
	widgetspresenter.h \
	ipresentingview.h \
	widgetspresenter_p.h

SOURCES += \
	widgetspresenter.cpp

TRANSLATIONS += \
	translations/qtmvvmwidgets_de.ts \
	translations/qtmvvmwidgets_template.ts

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

qpmx_ts_target.files -= $$OUT_PWD/$$QPMX_WORKINGDIR/qtmvvmwidgets_template.qm
qpmx_ts_target.files += translations/qtmvvmwidgets_template.ts

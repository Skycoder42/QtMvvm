TARGET = QtMvvmDataSyncWidgets

QT = core gui mvvmdatasynccore mvvmwidgets mvvmdatasynccore-private mvvmwidgets-private

HEADERS += \
	qtmvvmdatasyncwidgets_global.h \
	datasyncwindow.h \
	datasyncwindow_p.h \
	exportsetupdialog_p.h \
	changeremotedialog_p.h \
	identityeditdialog_p.h \
	networkexchangewindow.h \
	networkexchangewindow_p.h

SOURCES += \
	datasyncwindow.cpp \
	qtmvvmdatasyncwidgets_global.cpp \
	exportsetupdialog.cpp \
	changeremotedialog.cpp \
	identityeditdialog.cpp \
	networkexchangewindow.cpp

FORMS += \
	datasyncwindow.ui \
	exportsetupdialog.ui \
	changeremotedialog.ui \
	identityeditdialog.ui \
	networkexchangewindow.ui

TRANSLATIONS += \
	translations/qtmvvmdatasyncwidgets_de.ts \
	translations/qtmvvmdatasyncwidgets_template.ts

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

qpmx_ts_target.files -= $$OUT_PWD/$$QPMX_WORKINGDIR/qtmvvmdatasyncwidgets_template.qm
qpmx_ts_target.files += translations/qtmvvmdatasyncwidgets_template.ts

RESOURCES += \
    qtmvvmdatasyncwidgets_module.qrc


TARGET = QtMvvmDataSyncCore

QT = core gui mvvmcore datasync mvvmcore-private

HEADERS += \
	qtmvvmdatasynccore_global.h \
	datasyncviewmodel.h \
	datasyncviewmodel_p.h \
	accountmodel.h \
	accountmodel_p.h \
	exportsetupviewmodel_p.h \
	changeremoteviewmodel_p.h \
	identityeditviewmodel_p.h \
	networkexchangeviewmodel.h \
	networkexchangeviewmodel_p.h \
	exchangedevicesmodel.h \
	exchangedevicesmodel_p.h

SOURCES += \
	datasyncviewmodel.cpp \
	accountmodel.cpp \
	exportsetupviewmodel.cpp \
	changeremoteviewmodel.cpp \
	identityeditviewmodel.cpp \
	networkexchangeviewmodel.cpp \
	exchangedevicesmodel.cpp

TRANSLATIONS += \
	translations/qtmvvmdatasynccore_de.ts \
	translations/qtmvvmdatasynccore_template.ts

DISTFILES += $$TRANSLATIONS \
	application-x-datasync-account-data.xml

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

qpmx_ts_target.files -= $$OUT_PWD/$$QPMX_WORKINGDIR/qtmvvmdatasynccore_template.qm
qpmx_ts_target.files += translations/qtmvvmdatasynccore_template.ts

# source include for lupdate
never_true_for_lupdate {
	SOURCES += $$files(../imports/mvvmdatasynccore/*.cpp) \
		$$files(../imports/mvvmdatasynccore/*.qml)
}



TARGET = QtMvvmCore

QT = core gui
MODULE_CONFIG += qsettingstranslator

HEADERS += \
	viewmodel.h \
	qtmvvmcore_global.h \
	viewmodel_p.h \
	coreapp.h \
	coreapp_p.h \
	serviceregistry.h \
	serviceregistry_p.h \
	qtmvvmcore_helpertypes.h \
	ipresenter.h \
	qtmvvm_logging_p.h \
	binding.h \
	binding_p.h \
	message.h \
	message_p.h \
	settingssetup.h \
	settingssetuploader_p.h \
	settingsviewmodel_p.h \
	settingsviewmodel.h

SOURCES += \
	viewmodel.cpp \
	coreapp.cpp \
	serviceregistry.cpp \
	qtmvvmcore_global.cpp \
	binding.cpp \
	message.cpp \
	ipresenter.cpp \
	settingssetuploader.cpp \
	settingsviewmodel.cpp

TRANSLATIONS += \
	translations/qtmvvmcore_de.ts \
	translations/qtmvvmcore_template.ts

DISTFILES += $$TRANSLATIONS \
	settings.xsd

qpmx_ts_target.path = $$[QT_INSTALL_TRANSLATIONS]
qpmx_ts_target.depends += lrelease

load(qt_module)

FEATURES += ../../mkspecs/features/qsettingstranslator.prf
features.files = $$FEATURES
features.path = $$[QT_HOST_DATA]/mkspecs/features/

SCRIPTS += ../../bin/qsettingstranslator.py
scripts.files = $$SCRIPTS
scripts.path = $$[QT_INSTALL_BINS]

INSTALLS += features scripts

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


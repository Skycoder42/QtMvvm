TARGET = QtMvvmCore

QT = core gui
MODULE_CONFIG += qsettingsgenerator qsettingstranslator

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
	settingsviewmodel.h \
	injection.h \
	isettingsaccessor.h \
	qsettingsaccessor.h \
	settingsentry.h

SOURCES += \
	viewmodel.cpp \
	coreapp.cpp \
	serviceregistry.cpp \
	qtmvvmcore_global.cpp \
	binding.cpp \
	message.cpp \
	ipresenter.cpp \
	settingssetuploader.cpp \
	settingsviewmodel.cpp \
	isettingsaccessor.cpp \
	qsettingsaccessor.cpp \
	settingsentry.cpp

TRANSLATIONS += \
	translations/qtmvvmcore_de.ts \
	translations/qtmvvmcore_template.ts

DISTFILES += $$TRANSLATIONS

load(qt_module)

qpmx_ts_target.path = $$[QT_INSTALL_TRANSLATIONS]
qpmx_ts_target.depends += lrelease

FEATURES += \
	../../mkspecs/features/qsettingsgenerator.prf \
	../../mkspecs/features/qsettingstranslator.prf

features.files = $$FEATURES
features.path = $$[QT_HOST_DATA]/mkspecs/features/

INSTALLS += qpmx_ts_target features

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

# source include for lupdate
never_true_for_lupdate {
	SOURCES += $$files(../imports/mvvmcore/*.cpp) \
		$$files(../imports/mvvmcore/*.qml)
}

mingw: LIBS_PRIVATE += -lQt5Gui -lQt5Core

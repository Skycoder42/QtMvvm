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
	exchangedevicesmodel_p.h \
	datasyncsettingsviewmodel.h \
	datasyncsettingsaccessor.h \
	datasyncsettingsentry.h \
	datasyncsettingsaccessor_p.h

SOURCES += \
	datasyncviewmodel.cpp \
	accountmodel.cpp \
	exportsetupviewmodel.cpp \
	changeremoteviewmodel.cpp \
	identityeditviewmodel.cpp \
	networkexchangeviewmodel.cpp \
	exchangedevicesmodel.cpp \
	datasyncsettingsviewmodel.cpp \
	datasyncsettingsaccessor.cpp \
	datasyncsettingsentry.cpp

TRANSLATIONS += \
	translations/qtmvvmdatasynccore_de.ts \
	translations/qtmvvmdatasynccore_template.ts

DISTFILES += \
	application-x-datasync-account-data.xml

CONFIG += lrelease
QM_FILES_INSTALL_PATH = $$[QT_INSTALL_TRANSLATIONS]

load(qt_module)

win32 {
	QMAKE_TARGET_PRODUCT = "$$TARGET"
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "com.skycoder42."
}

android: QDEP_DEPENDS += Skycoder42/AndroidContentDevice@1.1.0

# source include for lupdate
never_true_for_lupdate {
	SOURCES += $$files(../imports/mvvmdatasynccore/*.cpp) \
		$$files(../imports/mvvmdatasynccore/*.qml)
}

!load(qdep):error("Failed to load qdep feature! Run 'qdep prfgen --qmake $$QMAKE_QMAKE' to create it.")

#replace template qm by ts
QM_FILES -= $$__qdep_lrelease_real_dir/qtmvvmdatasynccore_template.qm
QM_FILES += translations/qtmvvmdatasynccore_template.ts




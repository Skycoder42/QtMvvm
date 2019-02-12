TARGET = QtMvvmDataSyncQuick

QT = core gui quick mvvmdatasynccore mvvmquick mvvmdatasynccore-private

HEADERS += \
	qtmvvmdatasyncquick_global.h

SOURCES += \
	qtmvvmdatasyncquick_global.cpp

RESOURCES += \
	qtmvvmdatasyncquick_module.qrc

TRANSLATIONS += \
	translations/qtmvvmdatasyncquick_de.ts \
	translations/qtmvvmdatasyncquick_template.ts

load(qt_module)

CONFIG += lrelease
QM_FILES_INSTALL_PATH = $$[QT_INSTALL_TRANSLATIONS]

win32 {
	QMAKE_TARGET_PRODUCT = "$$TARGET"
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "com.skycoder42."
}

# source include for lupdate
never_true_for_lupdate {
	SOURCES += $$files(../imports/mvvmdatasyncquick/*.cpp) \
		$$files(../imports/mvvmdatasyncquick/*.qml)
}

!load(qdep):error("Failed to load qdep feature! Run 'qdep prfgen --qmake $$QMAKE_QMAKE' to create it.")

#replace template qm by ts
QM_FILES -= $$__qdep_lrelease_real_dir/qtmvvmdatasyncquick_template.qm
QM_FILES += translations/qtmvvmdatasyncquick_template.ts


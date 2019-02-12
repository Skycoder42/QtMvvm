TARGET = QtMvvmQuick

QT = core gui qml quick svg mvvmcore mvvmcore-private

HEADERS += \
	qtmvvmquick_global.h \
	quickpresenter.h \
	quickpresenter_p.h \
	inputviewfactory.h \
	inputviewfactory_p.h \
	formatters_p.h

SOURCES += \
	quickpresenter.cpp \
	inputviewfactory.cpp

RESOURCES += \
	qtmvvmquick_module.qrc

TRANSLATIONS += \
	translations/qtmvvmquick_de.ts \
	translations/qtmvvmquick_template.ts

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

QDEP_DEPENDS += \
	Skycoder42/QUrlValidator@1.2.0

# source include for lupdate
never_true_for_lupdate {
	SOURCES += $$files(../imports/mvvmquick/*.cpp) \
		$$files(../imports/mvvmquick/*.qml)
}

!load(qdep):error("Failed to load qdep feature! Run 'qdep prfgen --qmake $$QMAKE_QMAKE' to create it.")

#replace template qm by ts
QM_FILES -= $$__qdep_lrelease_real_dir/qtmvvmquick_template.qm
QM_FILES += translations/qtmvvmquick_template.ts

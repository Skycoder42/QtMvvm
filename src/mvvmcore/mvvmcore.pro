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
	settingsviewmodel_p.h \
	settingsviewmodel.h \
	injection.h \
	isettingsaccessor.h \
	qsettingsaccessor.h \
	settingsentry.h \
	settingsconfigloader_p.h \
    exception.h

SOURCES += \
	viewmodel.cpp \
	coreapp.cpp \
	serviceregistry.cpp \
	qtmvvmcore_global.cpp \
	binding.cpp \
	message.cpp \
	ipresenter.cpp \
	settingsviewmodel.cpp \
	isettingsaccessor.cpp \
	qsettingsaccessor.cpp \
	settingsentry.cpp \
	settingsconfigloader.cpp

android {
	QT += androidextras

	HEADERS += \
		androidsettingsaccessor.h \
		androidsettingsaccessor_p.h
	SOURCES += \
		androidsettingsaccessor.cpp

	ANDROID_BUNDLED_JAR_DEPENDENCIES = \
		jar/QtMvvmCore.jar
}

include(../settingsconfig/settingsconfig.pri)

TRANSLATIONS += \
	translations/qtmvvmcore_de.ts \
	translations/qtmvvmcore_template.ts

load(qt_module)
lib_bundle: FRAMEWORK_HEADERS.files += $$absolute_path(ViewModel, $$INC_PATH/include/$$MODULE_INCNAME)
else: gen_headers.files += $$absolute_path(ViewModel, $$INC_PATH/include/$$MODULE_INCNAME)

FEATURES += \
	../../mkspecs/features/qsettingsgenerator.prf \
	../../mkspecs/features/qsettingstranslator.prf

features.files = $$FEATURES
features.path = $$[QT_HOST_DATA]/mkspecs/features/

INSTALLS += features

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
	Skycoder42/QPluginFactory@1.5.0

# source include for lupdate
never_true_for_lupdate {
	SOURCES += $$files(../imports/mvvmcore/*.cpp) \
		$$files(../imports/mvvmcore/*.qml)
}

!load(qdep):error("Failed to load qdep feature! Run 'qdep prfgen --qmake $$QMAKE_QMAKE' to create it.")

#replace template qm by ts
QM_FILES -= $$__qdep_lrelease_real_dir/qtmvvmcore_template.qm
QM_FILES += translations/qtmvvmcore_template.ts

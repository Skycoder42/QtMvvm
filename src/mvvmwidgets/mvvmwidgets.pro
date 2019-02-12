TARGET = QtMvvmWidgets

QT = core gui widgets mvvmcore mvvmcore-private

HEADERS += \
	qtmvvmwidgets_global.h \
	widgetspresenter.h \
	ipresentingview.h \
	widgetspresenter_p.h \
	fontcombobox_p.h \
	selectcombobox_p.h \
	inputwidgetfactory.h \
	inputwidgetfactory_p.h \
	settingsdialog_p.h \
	settingsdialog.h \
	tooltipslider_p.h \
	coloredit_p.h \
	progressdialog_p.h

SOURCES += \
	widgetspresenter.cpp \
	fontcombobox.cpp \
	selectcombobox.cpp \
	inputwidgetfactory.cpp \
	settingsdialog.cpp \
	tooltipslider.cpp \
	coloredit.cpp \
	progressdialog.cpp

FORMS += \
	settingsdialog.ui

RESOURCES += \
	qtmvvmwidgets_module.qrc

TRANSLATIONS += \
	translations/qtmvvmwidgets_de.ts \
	translations/qtmvvmwidgets_template.ts

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

QDEP_DEPENDS += \
	Skycoder42/DialogMaster@1.4.0 \
	Skycoder42/QUrlValidator@1.2.0

!load(qdep):error("Failed to load qdep feature! Run 'qdep prfgen --qmake $$QMAKE_QMAKE' to create it.")

#replace template qm by ts
QM_FILES -= $$__qdep_lrelease_real_dir/qtmvvmwidgets_template.qm
QM_FILES += translations/qtmvvmwidgets_template.ts

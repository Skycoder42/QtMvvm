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

RESOURCES += \
	qtmvvmdatasyncwidgets_module.qrc

TRANSLATIONS += \
	translations/qtmvvmdatasyncwidgets_de.ts \
	translations/qtmvvmdatasyncwidgets_template.ts

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

!load(qdep):error("Failed to load qdep feature! Run 'qdep prfgen --qmake $$QMAKE_QMAKE' to create it.")

#replace template qm by ts
QM_FILES -= $$__qdep_lrelease_real_dir/qtmvvmdatasyncwidgets_template.qm
QM_FILES += translations/qtmvvmdatasyncwidgets_template.ts

QT += core qml quick mvvmquick
CXX_MODULE = mvvmquick
TARGETPATH = de/skycoder42/qtmvvm/quick
TARGET  = declarative_mvvmquick
IMPORT_VERSION = 1.0

HEADERS += \
	qtmvvmquick_plugin.h \
	qqmlquickpresenter.h

SOURCES += \
	qtmvvmquick_plugin.cpp \
	qqmlquickpresenter.cpp

QML_FILES += \
	QtMvvmApp.qml \
	PresentingStackView.qml \
	PresenterProgress.qml

OTHER_FILES += qmldir

generate_qmltypes {
	typeextra1.target = qmltypes
	typeextra1.depends += export LD_LIBRARY_PATH := "$$shadowed($$dirname(_QMAKE_CONF_))/lib/:$(LD_LIBRARY_PATH)"
	typeextra2.target = qmltypes
	typeextra2.depends += export QML2_IMPORT_PATH := "$$shadowed($$dirname(_QMAKE_CONF_))/qml/"
	QMAKE_EXTRA_TARGETS += typeextra1 typeextra2
}

load(qml_plugin)

generate_qmltypes {
	qmltypes.depends = ../../../qml/$$TARGETPATH/$(TARGET)  #overwrite the target deps

	mfirst.target = all
	mfirst.depends += qmltypes
	QMAKE_EXTRA_TARGETS += mfirst
}

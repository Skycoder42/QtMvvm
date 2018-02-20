TEMPLATE = app

QT  += core gui qml quick mvvmquick widgets

TARGET = SampleQuick

SOURCES += main.cpp \
	quickeventservice.cpp

RESOURCES += qml.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/mvvmquick/$$TARGET
INSTALLS += target

#not found by linker?
unix:!mac {
	LIBS += -L$$OUT_PWD/../../../lib #required to make this the first place to search
	LIBS += -L$$[QT_INSTALL_LIBS] -licudata
	LIBS += -L$$[QT_INSTALL_LIBS] -licui18n
	LIBS += -L$$[QT_INSTALL_LIBS] -licuuc
}

#add lib dir to rpath
mac: QMAKE_LFLAGS += '-Wl,-rpath,\'$$OUT_PWD/../../../lib\''

#link to core lib
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../mvvmcore/SampleCore/release/ -lSampleCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../mvvmcore/SampleCore/debug/ -lSampleCore
else:unix: LIBS += -L$$OUT_PWD/../../mvvmcore/SampleCore/ -lSampleCore

INCLUDEPATH += $$PWD/../../mvvmcore/SampleCore
DEPENDPATH += $$PWD/../../mvvmcore/SampleCore

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../mvvmcore/SampleCore/release/libSampleCore.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../mvvmcore/SampleCore/debug/libSampleCore.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../mvvmcore/SampleCore/release/SampleCore.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../mvvmcore/SampleCore/debug/SampleCore.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../mvvmcore/SampleCore/libSampleCore.a

#hacky code to make it possible to use the example from within a shadowed build
samples_in_build {
	# first, create a fake qml imports dir
	FAKEPATH = qml/de/skycoder42/QtMvvm/Quick
	ORIGPATH = ../../../../../../../../qml/de/skycoder42/QtMvvm/Quick
	QMLDEPPATH = $$PWD/../../../src/imports/mvvmquick
	system($$QMAKE_MKDIR $$shell_quote($$shell_path($$FAKEPATH)))

	# next, symlink all "compiled" files (whole dir for core, as it has no qml files
	build_symlink_target.target = create_qml_build_symlinks
	build_symlink_target.commands += $$QMAKE_SYMBOLIC_LINK $$shell_path(../../../../../../../qml/de/skycoder42/QtMvvm/Core) $$shell_path(qml/de/skycoder42/QtMvvm/Core) \
		$$escape_expand(\n\t)$$QMAKE_SYMBOLIC_LINK $$shell_path($$ORIGPATH/libdeclarative_mvvmquick.so) $$shell_path($$FAKEPATH/libdeclarative_mvvmquick.so) \
		$$escape_expand(\n\t)$$QMAKE_SYMBOLIC_LINK $$shell_path($$ORIGPATH/plugins.qmltypes) $$shell_path($$FAKEPATH/plugins.qmltypes) \
		$$escape_expand(\n\t)$$QMAKE_SYMBOLIC_LINK $$shell_path($$ORIGPATH/qmldir) $$shell_path($$FAKEPATH/qmldir)
	QMAKE_EXTRA_TARGETS += build_symlink_target

	# next, prepare compiler to symlink all the qml files
	qml_symlink_compiler.name = symlink ${QMAKE_FILE_IN}
	qml_symlink_compiler.input = QML_MODULE_DEPFILES
	qml_symlink_compiler.variable_out = QML_MODULE_DEPFILES_LINKS
	qml_symlink_compiler.output = $$FAKEPATH/${QMAKE_FILE_BASE}.qml
	qml_symlink_compiler.commands = $$QMAKE_SYMBOLIC_LINK $$QMLDEPPATH/${QMAKE_FILE_BASE}.qml ${QMAKE_FILE_OUT}
	qml_symlink_compiler.depends = create_qml_build_symlinks
	qml_symlink_compiler.CONFIG += no_link target_predeps
	QMAKE_EXTRA_COMPILERS += qml_symlink_compiler

	#finally, add the files to make it work
	QML_MODULE_DEPFILES += $$files($$QMLDEPPATH/*.qml)

	# also, add it as import path for QtCreator and the src files
	QML_IMPORT_PATH = $$OUT_PWD/qml/
	DEFINES += QML_PATH=\\\"$$QML_IMPORT_PATH\\\"
}

HEADERS += \
	quickeventservice.h

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)

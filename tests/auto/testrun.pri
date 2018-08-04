win32:!ReleaseBuild:!DebugBuild {
	runtarget.target = run-tests
	runtarget.CONFIG = recursive
	runtarget.recurse_target = run-tests
	QMAKE_EXTRA_TARGETS += runtarget
} else {
	win32:!win32-g++ {
		oneshell.target = .ONESHELL

		CONFIG(debug, debug|release): outdir_helper = debug
		CONFIG(release, debug|release): outdir_helper = release
		runtarget.target = run-tests
		runtarget.depends += $(DESTDIR_TARGET)
		runtarget.commands += @set PATH=$$shell_path($$shadowed($$dirname(_QMAKE_CONF_))/bin);$$shell_path($$[QT_INSTALL_BINS]);$(PATH)
		runtarget.commands += $$escape_expand(\\n\\t)@set QT_PLUGIN_PATH=$$shadowed($$dirname(_QMAKE_CONF_))/plugins;$(QT_PLUGIN_PATH)
		runtarget.commands += $$escape_expand(\\n\\t)@set QML2_IMPORT_PATH=$$shadowed($$dirname(_QMAKE_CONF_))/qml;$(QML2_IMPORT_PATH)
		runtarget.commands += $$escape_expand(\\n\\t)@if exist $${outdir_helper}\\fail del $${outdir_helper}\\fail
		runtarget.commands += $$escape_expand(\\n\\t)start /w call $(DESTDIR_TARGET) ^> $${outdir_helper}\\test.log ^|^| echo FAIL ^> $${outdir_helper}\\fail ^& exit 0
		runtarget.commands += $$escape_expand(\\n\\t)type $${outdir_helper}\\test.log
		runtarget.commands += $$escape_expand(\\n\\t)@if exist $${outdir_helper}\\fail exit 42
		QMAKE_EXTRA_TARGETS += oneshell runtarget
	} else {
		linux|win32-g++ {
			runtarget_env_lib.target = run-tests
			runtarget_env_lib.depends = export LD_LIBRARY_PATH := $$shadowed($$dirname(_QMAKE_CONF_))/lib/:$$[QT_INSTALL_LIBS]:$(LD_LIBRARY_PATH)
			runtarget_env_qpa.target = run-tests
			runtarget_env_qpa.depends += export QT_QPA_PLATFORM := minimal
			QMAKE_EXTRA_TARGETS += runtarget_env_lib
			!win32-g++: QMAKE_EXTRA_TARGETS += runtarget_env_qpa
		} else:mac {
			runtarget_env_lib.target = run-tests
			runtarget_env_lib.depends = export DYLD_LIBRARY_PATH := $$shadowed($$dirname(_QMAKE_CONF_))/lib/:$$[QT_INSTALL_LIBS]:$(DYLD_LIBRARY_PATH)
			runtarget_env_framework.target = run-tests
			runtarget_env_framework.depends = export DYLD_FRAMEWORK_PATH := $$shadowed($$dirname(_QMAKE_CONF_))/lib/:$$[QT_INSTALL_LIBS]:$(DYLD_FRAMEWORK_PATH)
			QMAKE_EXTRA_TARGETS += \
				runtarget_env_lib \
				runtarget_env_framework
		}

		runtarget_env_bin.target = run-tests
		runtarget_env_bin.depends = export PATH := $$shadowed($$dirname(_QMAKE_CONF_))/bin/:$$[QT_INSTALL_BINS]:$(PATH)
		runtarget_env_plugins.target = run-tests
		runtarget_env_plugins.depends = export QT_PLUGIN_PATH := $$shadowed($$dirname(_QMAKE_CONF_))/plugins/$${QMAKE_DIRLIST_SEP}$(QT_PLUGIN_PATH)
		runtarget_env_qml.target = run-tests
		runtarget_env_qml.depends += export QML2_IMPORT_PATH := $$shadowed($$dirname(_QMAKE_CONF_))/qml/$${QMAKE_DIRLIST_SEP}$(QML2_IMPORT_PATH)
		QMAKE_EXTRA_TARGETS += \
			runtarget_env_bin \
			runtarget_env_plugins \
			runtarget_env_qml

		runtarget.target = run-tests
		win32-g++ {
			runtarget.depends += $(DESTDIR_TARGET)
			runtarget.commands = ./$(DESTDIR_TARGET)
		} else {
			runtarget.depends += $(TARGET)
			runtarget.commands = ./$(TARGET)
		}
		QMAKE_EXTRA_TARGETS += runtarget
	}
}

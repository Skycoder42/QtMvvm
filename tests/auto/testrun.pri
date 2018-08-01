linux {
	runtarget_env_bin.target = run-tests
	runtarget_env_bin.depends = export PATH := "$$shadowed($$dirname(_QMAKE_CONF_))/bin/:$$[QT_INSTALL_BINS]:$(PATH)"
	runtarget_env_lib.target = run-tests
	runtarget_env_lib.depends = export LD_LIBRARY_PATH := "$$shadowed($$dirname(_QMAKE_CONF_))/lib/:$$[QT_INSTALL_LIBS]:$(LD_LIBRARY_PATH)"
	runtarget_env_plugins.target = run-tests
	runtarget_env_plugins.depends = export QT_PLUGIN_PATH := "$$shadowed($$dirname(_QMAKE_CONF_))/plugins/:$(QT_PLUGIN_PATH)"
	runtarget_env_qml.target = run-tests
	runtarget_env_qml.depends += export QML2_IMPORT_PATH := "$$shadowed($$dirname(_QMAKE_CONF_))/qml/:$(QML2_IMPORT_PATH)"
	runtarget_env_qpa.target = run-tests
	runtarget_env_qpa.depends += export QT_QPA_PLATFORM := "minimal"
	QMAKE_EXTRA_TARGETS += \
		runtarget_env_bin \
		runtarget_env_lib \
		runtarget_env_plugins \
		runtarget_env_qml\
		runtarget_env_qpa
} else:mac {
	runtarget_env_bin.target = run-tests
	runtarget_env_bin.depends = export PATH := "$$shadowed($$dirname(_QMAKE_CONF_))/bin/:$$[QT_INSTALL_BINS]:$(PATH)"
	runtarget_env_lib.target = run-tests
	runtarget_env_lib.depends = export DYLD_LIBRARY_PATH := "$$shadowed($$dirname(_QMAKE_CONF_))/lib/:$$[QT_INSTALL_LIBS]:$(DYLD_LIBRARY_PATH)"
	runtarget_env_framework.target = run-tests
	runtarget_env_framework.depends = export DYLD_FRAMEWORK_PATH := "$$shadowed($$dirname(_QMAKE_CONF_))/lib/:$$[QT_INSTALL_LIBS]:$(DYLD_FRAMEWORK_PATH)"
	runtarget_env_plugins.target = run-tests
	runtarget_env_plugins.depends = export QT_PLUGIN_PATH := "$$shadowed($$dirname(_QMAKE_CONF_))/plugins/:$(QT_PLUGIN_PATH)"
	runtarget_env_qml.target = run-tests
	runtarget_env_qml.depends += export QML2_IMPORT_PATH := "$$shadowed($$dirname(_QMAKE_CONF_))/qml/:$(QML2_IMPORT_PATH)"
	QMAKE_EXTRA_TARGETS += \
		runtarget_env_bin \
		runtarget_env_lib \
		runtarget_env_framework \
		runtarget_env_plugins \
		runtarget_env_qml
} else:win32 {
	runtarget_env_bin.target = run-tests
	runtarget_env_bin.depends = export PATH := "$$shadowed($$dirname(_QMAKE_CONF_))\bin\;$$shell_path($$[QT_INSTALL_BINS]);$(PATH)"
	runtarget_env_plugins.target = run-tests
	runtarget_env_plugins.depends = export QT_PLUGIN_PATH := "$$shadowed($$dirname(_QMAKE_CONF_))/plugins/;$(QT_PLUGIN_PATH)"
	runtarget_env_qml.target = run-tests
	runtarget_env_qml.depends += export QML2_IMPORT_PATH := "$$shadowed($$dirname(_QMAKE_CONF_))/qml/;$(QML2_IMPORT_PATH)"
	runtarget_env_qpa.target = run-tests
	runtarget_env_qpa.depends += export QT_QPA_PLATFORM := "minimal"
	QMAKE_EXTRA_TARGETS += \
		runtarget_env_bin \
		runtarget_env_plugins \
		runtarget_env_qml\
		runtarget_env_qpa
}

runtarget.target = run-tests
runtarget.depends += $(TARGET)
win32: runtarget.commands = .\$(TARGET)
else: runtarget.commands = ./$(TARGET)
QMAKE_EXTRA_TARGETS += runtarget

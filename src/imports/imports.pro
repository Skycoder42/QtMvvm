TEMPLATE = subdirs

SUBDIRS += \
	mvvmcore \
	mvvmquick

mvvmquick.depends += mvvmcore

qtHaveModule(datasync) {
	SUBDIRS += mvvmdatasynccore \
		mvvmdatasyncquick

	mvvmdatasynccore.depends += mvvmcore
	mvvmdatasyncquick.depends += mvvmdatasynccore mvvmquick
}

TEMPLATE = subdirs

SUBDIRS += \
	mvvmcore \
	mvvmquick

qtHaveModule(datasync) {
	SUBDIRS += mvvmdatasynccore \
		mvvmdatasyncquick
}

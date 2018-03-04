TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
	mvvmcore \
	mvvmquick

qtHaveModule(datasync) {
	SUBDIRS += mvvmdatasynccore \
		mvvmdatasyncquick
}

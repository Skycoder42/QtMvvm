TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
	mvvmquick \
	mvvmcore

qtHaveModule(datasync) {
	SUBDIRS += mvvmdatasynccore \
		mvvmdatasyncquick
}

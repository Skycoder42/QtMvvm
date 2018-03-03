TEMPLATE = subdirs

SUBDIRS += \
	mvvmquick \
	mvvmcore

qtHaveModule(datasync) {
	SUBDIRS += mvvmdatasynccore
}

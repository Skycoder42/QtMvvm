TEMPLATE = subdirs

SUBDIRS = mvvmcore \
	mvvmwidgets \
	mvvmquick

qtHaveModule(datasync) {
	SUBDIRS += mvvmdatasynccore \
		mvvmdatasyncwidgets \
		mvvmdatasyncquick
}

CONFIG += ordered

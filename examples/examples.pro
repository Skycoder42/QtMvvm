TEMPLATE = subdirs

SUBDIRS = mvvmcore \
	mvvmwidgets \
	mvvmquick \
    mvvmdatasyncquick

qtHaveModule(datasync) {
	SUBDIRS += mvvmdatasynccore \
		mvvmdatasyncwidgets
}

CONFIG += ordered

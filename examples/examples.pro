TEMPLATE = subdirs

SUBDIRS = mvvmcore \
	mvvmwidgets \
	mvvmquick

mvvmwidgets.depends += mvvmcore
mvvmquick.depends += mvvmcore

qtHaveModule(datasync) {
	SUBDIRS += mvvmdatasynccore \
		mvvmdatasyncwidgets \
		mvvmdatasyncquick

	mvvmdatasyncwidgets.depends += mvvmdatasynccore
	mvvmdatasyncquick.depends += mvvmdatasynccore
}

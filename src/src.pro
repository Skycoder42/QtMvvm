TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += mvvmcore \
	mvvmwidgets \
	mvvmquick

qtHaveModule(datasync) {
	SUBDIRS += mvvmdatasynccore \
		mvvmdatasyncwidgets \
		mvvmdatasyncquick
}

SUBDIRS += imports

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease

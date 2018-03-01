TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += mvvmcore \
	mvvmwidgets \
	mvvmquick \
	imports

qtHaveModule(datasync) {
	SUBDIRS += mvvmdatasynccore \
		mvvmdatasyncwidgets
}

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease

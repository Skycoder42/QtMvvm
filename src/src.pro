TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += mvvmcore \
	mvvmwidgets \
	mvvmquick \
    imports

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease

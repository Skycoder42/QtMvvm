TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += mvvmcore \
	mvvmwidgets \
	mvvmquick

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease

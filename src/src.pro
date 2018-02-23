TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += mvvmcore \
	mvvmwidgets \
	mvvmquick \
	imports \
    mvvmsettingscore \
    mvvmsettingswidgets

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease

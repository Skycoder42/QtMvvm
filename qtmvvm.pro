load(qt_parts)

SUBDIRS += doc

doxygen.target = doxygen
doxygen.CONFIG = recursive
doxygen.recurse_target = doxygen
doxygen.recurse += doc
QMAKE_EXTRA_TARGETS += doxygen

lrelease.target = lrelease
lrelease.CONFIG = recursive
lrelease.recurse_target = lrelease
lrelease.recurse += sub_src
QMAKE_EXTRA_TARGETS += lrelease

runtests.target = run-tests
runtests.CONFIG = recursive
runtests.recurse_target = run-tests
runtests.recurse += sub_tests sub_src sub_tools
sub_runtest.target = sub-tests-runtests
sub_runtest.depends += sub-tests
QMAKE_EXTRA_TARGETS += sub_runtest runtests

DISTFILES += .qmake.conf \
	sync.profile \
	qbs/Qt/settingsgenerator/*

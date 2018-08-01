TEMPLATE = subdirs

SUBDIRS += cmake \
	mvvmcore \
	qml

qtHaveModule(datasync) {
	SUBDIRS += \
		mvvmdatasynccore
}

cmake.CONFIG += no_run-tests_target
prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests

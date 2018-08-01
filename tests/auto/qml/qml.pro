TEMPLATE = subdirs

SUBDIRS += \
	qmlsettingsgenerator \
	qmlmvvmcore

equals(MSVC_VER, 14.0): SUBDIRS -= qmlsettingsgenerator

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests

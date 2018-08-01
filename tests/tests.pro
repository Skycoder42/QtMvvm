TEMPLATE = subdirs

CONFIG += no_docs_target

SUBDIRS += auto

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests

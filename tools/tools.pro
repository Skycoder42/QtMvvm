TEMPLATE = subdirs

SUBDIRS += \
	settingsgenerator

settingsgenerator.CONFIG += no_lrelease_target

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease

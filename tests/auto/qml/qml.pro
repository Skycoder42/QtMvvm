TEMPLATE = subdirs

SUBDIRS += \
	qmlsettingsgenerator

equals(MSVC_VER, 14.0): SUBDIRS -= qmlsettingsgenerator

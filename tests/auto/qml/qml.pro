TEMPLATE = subdirs

SUBDIRS += \
	qmlsettingsgenerator \
	qmlmvvmcore

equals(MSVC_VER, 14.0): SUBDIRS -= qmlsettingsgenerator

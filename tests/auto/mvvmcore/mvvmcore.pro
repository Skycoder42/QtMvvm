TEMPLATE = subdirs

SUBDIRS += \
	settingsgenerator \
	serviceregistry \
	serviceregistrytestplugin

serviceregistry.depends += serviceregistrytestplugin

equals(MSVC_VER, 14.0): SUBDIRS -= settingsgenerator

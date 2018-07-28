TEMPLATE = subdirs

SUBDIRS += \
	settingsgenerator \
	serviceregistry \
	serviceregistrytestplugin

serviceregistry.depends += serviceregistrytestplugin

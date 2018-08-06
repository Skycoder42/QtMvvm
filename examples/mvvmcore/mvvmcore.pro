TEMPLATE = subdirs
QT_FOR_CONFIG += core

SUBDIRS += \
	SampleCore

android: SUBDIRS += DroidSettings

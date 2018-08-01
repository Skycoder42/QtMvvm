TEMPLATE = subdirs

SUBDIRS += cmake \
	mvvmcore \
	qml

qtHaveModule(datasync) {
	SUBDIRS += \
		mvvmdatasynccore
}

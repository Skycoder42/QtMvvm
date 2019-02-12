HEADERS += \
	$$PWD/../3rdparty/optional-lite/optional.hpp \
	$$PWD/../3rdparty/variant-lite/variant.hpp \
	$$PWD/prefix_p.h \
	$$PWD/settingsconfigimpl_p.h

SOURCES += $$PWD/settingsconfigimpl.cpp

XML_SCHEMA_DEFINITIONS += $$PWD/settingsconfig.xsd

INCLUDEPATH += $$PWD $$PWD/../3rdparty/optional-lite $$PWD/../3rdparty/variant-lite

contains(QT, xmlpatterns): RESOURCES += \
	$$PWD/settingsconfig.qrc

QDEP_DEPENDS += \
	Skycoder42/QXmlCodeGen@1.4.1

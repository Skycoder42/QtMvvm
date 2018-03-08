TEMPLATE = lib

QT += mvvmcore
# Creating a static library is typically more efficient. You can still create a shared library if you want to
CONFIG += c++14 static

TARGET = %{CoreName}

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \\
	%{AppHdrName} \\
	%{VmHdrName}

SOURCES += \\
	%{AppSrcName} \\
	%{VmSrcName}

@if '%{UseSettings}'
RESOURCES += \\
	%{AppQrcFile}

@endif
TRANSLATIONS += %{ProjectLowerName}_core_de.ts \\
	%{ProjectLowerName}_core_template.ts

DISTFILES += $$TRANSLATIONS
@if '%{UseSettings}'
QTMVVM_TS_SETTINGS = settings.xml
_never_true_condition: SOURCES += $$files($$PWD/.ts-dummy/*)
# Uncomment the following line to automatically generated and update settings translations when building
#PRE_TARGETDEPS += qtmvvm-tsgen
@endif

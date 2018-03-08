TEMPLATE = app

QT += widgets mvvmwidgets
CONFIG += c++14

TARGET = %{WidgetsName}

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += %{WindowHdrName}

SOURCES += main.cpp \\
	%{WindowSrcName}

FORMS += %{WindowFormName}

TRANSLATIONS += %{ProjectLowerName}_widgets_de.ts \\
	%{ProjectLowerName}_widgets_template.ts

# Link with core project
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../%{CoreName}/release/ -l%{CoreName}
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../%{CoreName}/debug/ -l%{CoreName}
else:unix: LIBS += -L$$OUT_PWD/../%{CoreName}/ -l%{CoreName}

INCLUDEPATH += $$PWD/../%{CoreName}
DEPENDPATH += $$PWD/../%{CoreName}

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../%{CoreName}/release/lib%{CoreName}.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../%{CoreName}/debug/lib%{CoreName}.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../%{CoreName}/release/%{CoreName}.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../%{CoreName}/debug/%{CoreName}.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../%{CoreName}/lib%{CoreName}.a

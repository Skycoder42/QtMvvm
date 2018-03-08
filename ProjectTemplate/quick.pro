TEMPLATE = app

QT += core gui qml quick quickcontrols2
CONFIG += c++11

TARGET = %{QuickName}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp

RESOURCES += \\
    %{QuickQrcFile}

TRANSLATIONS += %{ProjectLowerName}_quick_de.ts \\
	%{ProjectLowerName}_quick_template.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

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

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)

QT       += core gui thelib network tdesktopenvironment

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# Include the-libs build tools
# Include the-libs build tools
equals(THELIBS_BUILDTOOLS_PATH, "") {
    THELIBS_BUILDTOOLS_PATH = $$[QT_INSTALL_PREFIX]/share/the-libs/pri
}
include($$THELIBS_BUILDTOOLS_PATH/gentranslations.pri)
include($$THELIBS_BUILDTOOLS_PATH/varset.pri)

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    crash/backtracedialog.cpp \
    crash/backtracepopover.cpp \
    crash/backtracewidget.cpp \
    crash/crashwidget.cpp \
    main.cpp \
    splash/splashcontroller.cpp \
    splash/splashwidget.cpp \
    splashwindow.cpp

HEADERS += \
    crash/backtracedialog.h \
    crash/backtracepopover.h \
    crash/backtracewidget.h \
    crash/crashwidget.h \
    splash/splashcontroller.h \
    splash/splashwidget.h \
    splashwindow.h

FORMS += \
    crash/backtracedialog.ui \
    crash/backtracepopover.ui \
    crash/backtracewidget.ui \
    crash/crashwidget.ui \
    splash/splashwidget.ui \
    splashwindow.ui

DISTFILES += \
    thedesk.desktop

unix {
    target.path = /usr/bin/

    translations.files = translations/*.qm
    translations.path = /usr/share/thedesk/startdesk/translations

    xsession.path = /usr/share/xsessions
    xsession.files = thedesk.desktop

    INSTALLS += target translations xsession
}

DEFINES += SYSTEM_LIBRARY_DIRECTORY=\\\"$$THELIBS_INSTALL_LIB\\\"

unix:!macx: LIBS += -L$$OUT_PWD/../libthedesk/ -lthedesk

INCLUDEPATH += $$PWD/../libthedesk
DEPENDPATH += $$PWD/../libthedesk

RESOURCES += \
    resources.qrc

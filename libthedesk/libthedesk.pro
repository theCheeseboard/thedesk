QT += widgets thelib tdesktopenvironment dbus

TEMPLATE = lib
DEFINES += LIBTHEDESK_LIBRARY
TARGET = thedesk

CONFIG += c++11

# Include the-libs build tools
include(/usr/share/the-libs/pri/gentranslations.pri)

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    application.cpp \
    barmanager.cpp \
    chunk.cpp \
    gatewaymanager.cpp \
    hudmanager.cpp \
    icontextchunk.cpp \
    keygrab.cpp \
    localemanager.cpp \
    onboardingmanager.cpp \
    onboardingpage.cpp \
    powermanager.cpp \
    private/localeselector.cpp \
    qsettingsformats.cpp \
    quickswitch.cpp \
    statemanager.cpp \
    statuscentermanager.cpp \
    statuscenterpane.cpp

HEADERS += \
    application.h \
    barmanager.h \
    chunk.h \
    gatewaymanager.h \
    hudmanager.h \
    icontextchunk.h \
    keygrab.h \
    libthedesk_global.h \
    localemanager.h \
    onboardingmanager.h \
    onboardingpage.h \
    powermanager.h \
    private/localeselector.h \
    private/onboardingmanager_p.h \
    qsettingsformats.h \
    quickswitch.h \
    statemanager.h \
    statuscentermanager.h \
    statuscenterpane.h

# Default rules for deployment.
unix {
    target.path = /usr/lib/

    headers.files = *.h
    headers.path = /usr/include/libthedesk/

    translations.files = translations/*.qm
    translations.path = /usr/share/thedesk/libthedesk/translations

    INSTALLS += target translations headers
}


FORMS += \
    icontextchunk.ui \
    private/localeselector.ui

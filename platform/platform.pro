QT += gui gui-private widgets thelib

TEMPLATE = lib
CONFIG += plugin
TARGET = thedesk-platform

CONFIG += c++11

unix {
    # Include the-libs build tools
    include(/usr/share/the-libs/pri/gentranslations.pri)

    CONFIG += link_pkgconfig

    packagesExist(x11) {
        message("Building with X11 support");
        PKGCONFIG += x11
        DEFINES += HAVE_X11
        QT += x11extras
    } else {
        message("X11 not found on this system.");
    }
}

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
    cursorhandler.cpp \
    fontformat.cpp \
    paletteformat.cpp \
    platformtheme.cpp \
    plugin.cpp

HEADERS += \
    cursorhandler.h \
    fontformat.h \
    paletteformat.h \
    platformtheme.h \
    plugin.h

DISTFILES += \
    palettes.json \
    platformdefaults.conf \
    plugin.json


unix {
    target.path = $$[QT_INSTALL_PLUGINS]/platformthemes/

    translations.path = /usr/share/thedesk/thedesk-platform/translations
    translations.files = translations/*

    defaults.files = platformdefaults.conf
    defaults.path = /etc/theSuite/theDesk/

    INSTALLS += target translations defaults
}

DEFINES += SYSTEM_LIBRARY_DIRECTORY=\\\"$$[QT_INSTALL_LIBS]\\\"

RESOURCES += \
    thedesk-platform-resources.qrc

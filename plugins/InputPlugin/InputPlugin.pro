QT += gui widgets

TEMPLATE = lib
CONFIG += plugin

CONFIG += c++11

# Include the-libs build tools
include(/usr/share/the-libs/pri/gentranslations.pri)

CONFIG += link_pkgconfig

packagesExist(x11) {
    message("Building with X11 support");
    PKGCONFIG += x11
    DEFINES += HAVE_X11
    QT += x11extras


    packagesExist(xi) : packagesExist(xorg-libinput) {
        PKGCONFIG += xi xorg-libinput
        DEFINES += HAVE_XI
        message("Building with XInput support");

        SOURCES += \
            backends/xinputbackend.cpp

        HEADERS += \
            backends/xinputbackend.h \
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
    backends/settingsbackend.cpp \
    chunks/keyboardlayoutchunk.cpp \
    daemons/keyboarddaemon.cpp \
    models/selectedkeyboardlayoutsmodel.cpp \
    plugin.cpp \
    settings/inputsettingsleftpane.cpp \
    settings/inputsettingspane.cpp \
    settings/keyboardsettings.cpp \
    settings/mousesettings.cpp

HEADERS += \
    backends/settingsbackend.h \
    chunks/keyboardlayoutchunk.h \
    daemons/keyboarddaemon.h \
    models/selectedkeyboardlayoutsmodel.h \
    plugin.h \
    settings/inputsettingsleftpane.h \
    settings/inputsettingspane.h \
    settings/keyboardsettings.h \
    settings/mousesettings.h

DISTFILES += \
    Plugin.json \
    defaults.conf


unix {
    translations.files = translations/*.qm
    translations.path = /usr/share/thedesk/InputPlugin/translations

    defaults.files = defaults.conf
    defaults.path = /etc/theSuite/theDesk/InputPlugin/

    INSTALLS += translations defaults
}

include(../plugins.pri)

FORMS += \
    chunks/keyboardlayoutchunk.ui \
    settings/inputsettingsleftpane.ui \
    settings/inputsettingspane.ui \
    settings/keyboardsettings.ui \
    settings/mousesettings.ui

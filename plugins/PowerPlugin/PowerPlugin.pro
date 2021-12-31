QT += widgets

TEMPLATE = lib
DEFINES += POWERPLUGIN_LIBRARY

# Include the-libs build tools
include(/usr/share/the-libs/pri/gentranslations.pri)

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

unix {
    translations.files = translations/*.qm
    translations.path = /usr/share/thedesk/PowerPlugin/translations

    defaults.files = defaults.conf
    defaults.path = /etc/theSuite/theDesk/PowerPlugin/

    INSTALLS += translations defaults
}

include(../plugins.pri)

HEADERS += \
    eventhandler.h \
    plugin.h \
    settings/powersettings.h \
    sleepmonitor.h

SOURCES += \
    eventhandler.cpp \
    plugin.cpp \
    settings/powersettings.cpp \
    sleepmonitor.cpp

DISTFILES += \
    Plugin.json \
    defaults.conf

FORMS += \
    settings/powersettings.ui

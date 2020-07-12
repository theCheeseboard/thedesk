QT += gui widgets twebservices

TEMPLATE = lib
CONFIG += plugin

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
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    arrange/arrangecontroller.cpp \
    arrange/arrangewidget.cpp \
    arrange/overlaywindow.cpp \
    plugin.cpp \
    redshift/colorramp.cpp \
    redshift/redshiftdaemon.cpp \
    settings/displaysettings.cpp

HEADERS += \
    arrange/arrangecontroller.h \
    arrange/arrangewidget.h \
    arrange/overlaywindow.h \
    plugin.h \
    redshift/colorramp.h \
    redshift/redshiftdaemon.h \
    settings/displaysettings.h

DISTFILES += \
    Plugin.json \
    defaults.conf

unix {
    translations.files = translations/*.qm
    translations.path = /usr/share/thedesk/DisplayPlugin/translations

    defaults.files = defaults.conf
    defaults.path = /etc/theSuite/theDesk/DisplayPlugin/

    INSTALLS += translations defaults
}

include(../plugins.pri)

FORMS += \
    arrange/arrangewidget.ui \
    arrange/overlaywindow.ui \
    settings/displaysettings.ui

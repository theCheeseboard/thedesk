CONFIG += qt

TEMPLATE = lib
DEFINES += STARTDESKPLUGIN_LIBRARY
TARGET = thedesk-startdesk

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    startdeskplugin.cpp

HEADERS += \
    startdeskplugin.h

include(../plugins.pri)

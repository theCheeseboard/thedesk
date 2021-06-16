CONFIG += qt

TEMPLATE = lib
DEFINES += KEYGRABPLUGIN_LIBRARY
TARGET = thedesk-keygrab

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    keygrabplugin.cpp

HEADERS += \
    keygrabplugin.h

WAYLAND_PROTOCOL_EXTENSIONS += /usr/share/libtdesktopenvironment/wayland-protocols/tdesktopenvironment-keygrab-v1.xml

include(../plugins.pri)

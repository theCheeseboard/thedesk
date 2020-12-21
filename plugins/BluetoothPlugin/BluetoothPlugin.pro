QT += gui widgets BluezQt

TEMPLATE = lib
CONFIG += plugin

CONFIG += c++11

# Include the-libs build tools
include(/usr/share/the-libs/pri/gentranslations.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    plugin.cpp \
    settings/bluetoothsettingspane.cpp

HEADERS += \
    plugin.h \
    settings/bluetoothsettingspane.h

DISTFILES += Plugin.json \
    defaults.conf

unix {
    translations.files = translations/*.qm
    translations.path = /usr/share/thedesk/BluetoothPlugin/translations

    defaults.files = defaults.conf
    defaults.path = /etc/theSuite/theDesk/BluetoothPlugin/

    INSTALLS += translations defaults
}

include(../plugins.pri)

FORMS += \
    settings/bluetoothsettingspane.ui


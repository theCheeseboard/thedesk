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
    btagent.cpp \
    chunk/bluetoothchunk.cpp \
    common.cpp \
    devicedelegate.cpp \
    plugin.cpp \
    settings/bluetoothsettingspane.cpp \
    settings/devicepopover.cpp \
    settings/pairpopover.cpp

HEADERS += \
    btagent.h \
    chunk/bluetoothchunk.h \
    common.h \
    devicedelegate.h \
    plugin.h \
    settings/bluetoothsettingspane.h \
    settings/devicepopover.h \
    settings/pairpopover.h

DISTFILES += Plugin.json \
    defaults.conf \
    defaults.json

unix {
    translations.files = translations/*.qm
    translations.path = /usr/share/thedesk/BluetoothPlugin/translations

    defaults.files = defaults.conf
    defaults.path = /etc/theSuite/theDesk/BluetoothPlugin/

    INSTALLS += translations defaults
}

include(../plugins.pri)

FORMS += \
    settings/bluetoothsettingspane.ui \
    settings/devicepopover.ui \
    settings/pairpopover.ui


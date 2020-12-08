QT += gui widgets network concurrent

TEMPLATE = lib
CONFIG += plugin

CONFIG += c++11

# Include the-libs build tools
include(/usr/share/the-libs/pri/gentranslations.pri)

#For some reason they got rid of QMake support, so we'll have to hack this in for now
qtHaveModule(ModemManagerQt) {
    QT += ModemManagerQt
} else {
    INCLUDEPATH += /usr/include/KF5/ModemManagerQt/ /usr/include/ModemManager/
    LIBS += -lKF5ModemManagerQt
}

qtHaveModule(NetworkManagerQt) {
    QT += NetworkManagerQt
} else {
    INCLUDEPATH += /usr/include/KF5/NetworkManagerQt/
    LIBS += -lKF5NetworkManagerQt
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
    chunk/chunkupdater.cpp \
    chunk/genericchunkupdater.cpp \
    chunk/networkchunk.cpp \
    chunk/wiredchunkupdater.cpp \
    chunk/wirelesschunkupdater.cpp \
    common.cpp \
    models/deviceconnectionlistmodel.cpp \
    models/wirelessaccesspointsmodel.cpp \
    models/wirelessconnectionlistmodel.cpp \
    models/wirelessnetworklistdelegate.cpp \
    plugin.cpp \
    statusCenter/connectionEditor/connectioneditorpane.cpp \
    statusCenter/connectionEditor/ipv4connectioneditorpane.cpp \
    statusCenter/connectionEditor/networkconnectioneditor.cpp \
    statusCenter/connectionEditor/wificonnectioneditorpane.cpp \
    statusCenter/devicePanes/abstractdevicepane.cpp \
    statusCenter/devicePanes/cellularpane.cpp \
    statusCenter/devicePanes/devicepane.cpp \
    statusCenter/devicePanes/wifidevicepane.cpp \
    statusCenter/devicePanes/wireddevicepane.cpp \
    statusCenter/eap/securityeap.cpp \
    statusCenter/eap/securityeappeap.cpp \
    statusCenter/eap/securityeapunsupported.cpp \
    statusCenter/networkstatuscenterleftpane.cpp \
    statusCenter/networkstatuscenterpane.cpp \
    statusCenter/popovers/connectionselectionpopover.cpp \
    statusCenter/popovers/simsettingspopover.cpp \
    statusCenter/popovers/tetheringsettingspopover.cpp \
    statusCenter/popovers/unlockmodempopover.cpp \
    statusCenter/popovers/wirelessnetworkselectionpopover.cpp \
    switchmanager.cpp

HEADERS += \
    chunk/chunkupdater.h \
    chunk/genericchunkupdater.h \
    chunk/networkchunk.h \
    chunk/wiredchunkupdater.h \
    chunk/wirelesschunkupdater.h \
    common.h \
    models/deviceconnectionlistmodel.h \
    models/wirelessaccesspointsmodel.h \
    models/wirelessconnectionlistmodel.h \
    models/wirelessnetworklistdelegate.h \
    plugin.h \
    statusCenter/connectionEditor/connectioneditorpane.h \
    statusCenter/connectionEditor/ipv4connectioneditorpane.h \
    statusCenter/connectionEditor/networkconnectioneditor.h \
    statusCenter/connectionEditor/wificonnectioneditorpane.h \
    statusCenter/devicePanes/abstractdevicepane.h \
    statusCenter/devicePanes/cellularpane.h \
    statusCenter/devicePanes/devicepane.h \
    statusCenter/devicePanes/wifidevicepane.h \
    statusCenter/devicePanes/wireddevicepane.h \
    statusCenter/eap/securityeap.h \
    statusCenter/eap/securityeappeap.h \
    statusCenter/eap/securityeapunsupported.h \
    statusCenter/networkstatuscenterleftpane.h \
    statusCenter/networkstatuscenterpane.h \
    statusCenter/popovers/connectionselectionpopover.h \
    statusCenter/popovers/simsettingspopover.h \
    statusCenter/popovers/tetheringsettingspopover.h \
    statusCenter/popovers/unlockmodempopover.h \
    statusCenter/popovers/wirelessnetworkselectionpopover.h \
    switchmanager.h

DISTFILES += \
    Plugin.json \
    defaults.conf

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += libnm libqrencode

    translations.files = translations/*.qm
    translations.path = /usr/share/thedesk/NetworkPlugin/translations

    defaults.files = defaults.conf
    defaults.path = /etc/theSuite/theDesk/NetworkPlugin/

    INSTALLS += translations defaults
}

include(../plugins.pri)

FORMS += \
    statusCenter/connectionEditor/ipv4connectioneditorpane.ui \
    statusCenter/connectionEditor/networkconnectioneditor.ui \
    statusCenter/connectionEditor/wificonnectioneditorpane.ui \
    statusCenter/devicePanes/cellularpane.ui \
    statusCenter/devicePanes/devicepane.ui \
    statusCenter/devicePanes/wifidevicepane.ui \
    statusCenter/devicePanes/wireddevicepane.ui \
    statusCenter/eap/securityeappeap.ui \
    statusCenter/eap/securityeapunsupported.ui \
    statusCenter/networkstatuscenterleftpane.ui \
    statusCenter/networkstatuscenterpane.ui \
    statusCenter/popovers/connectionselectionpopover.ui \
    statusCenter/popovers/simsettingspopover.ui \
    statusCenter/popovers/tetheringsettingspopover.ui \
    statusCenter/popovers/unlockmodempopover.ui \
    statusCenter/popovers/wirelessnetworkselectionpopover.ui

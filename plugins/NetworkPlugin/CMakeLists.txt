cmake_minimum_required(VERSION 3.4.0)

find_package(Qt6 REQUIRED COMPONENTS Widgets DBus)
find_package(libcontemporary)
find_package(KF5NetworkManagerQt REQUIRED)
find_package(KF5ModemManagerQt REQUIRED)
cntp_find_pkgconfig(qrencode libqrencode REQUIRED)

set(SOURCES
    chunk/chunkupdater.cpp
    chunk/genericchunkupdater.cpp
    chunk/networkchunk.cpp
    chunk/wiredchunkupdater.cpp
    chunk/wirelesschunkupdater.cpp
    networkplugincommon.cpp
    models/deviceconnectionlistmodel.cpp
    models/wirelessaccesspointsmodel.cpp
    models/wirelessconnectionlistmodel.cpp
    models/wirelessnetworklistdelegate.cpp
    onboarding/onboardingnetwork.cpp
    onboarding/wiredonboardingsetup.cpp
    onboarding/wirelessonboardingsetup.cpp
    plugin.cpp
    statusCenter/connectionEditor/connectioneditorpane.cpp
    statusCenter/connectionEditor/ipv4connectioneditorpane.cpp
    statusCenter/connectionEditor/networkconnectioneditor.cpp
    statusCenter/connectionEditor/wificonnectioneditorpane.cpp
    statusCenter/devicePanes/abstractdevicepane.cpp
    statusCenter/devicePanes/cellularpane.cpp
    statusCenter/devicePanes/devicepane.cpp
    statusCenter/devicePanes/wifidevicepane.cpp
    statusCenter/devicePanes/wireddevicepane.cpp
    statusCenter/eap/securityeap.cpp
    statusCenter/eap/securityeappeap.cpp
    statusCenter/eap/securityeapunsupported.cpp
    statusCenter/networkstatuscenterleftpane.cpp
    statusCenter/networkstatuscenterpane.cpp
    statusCenter/popovers/connectionselectionpopover.cpp
    statusCenter/popovers/simsettingspopover.cpp
    statusCenter/popovers/tetheringsettingspopover.cpp
    statusCenter/popovers/unlockmodempopover.cpp
    statusCenter/popovers/wirelessnetworkselectionpopover.cpp
    switchmanager.cpp

    onboarding/onboardingnetwork.ui
    onboarding/wiredonboardingsetup.ui
    onboarding/wirelessonboardingsetup.ui
    statusCenter/connectionEditor/ipv4connectioneditorpane.ui
    statusCenter/connectionEditor/networkconnectioneditor.ui
    statusCenter/connectionEditor/wificonnectioneditorpane.ui
    statusCenter/devicePanes/cellularpane.ui
    statusCenter/devicePanes/devicepane.ui
    statusCenter/devicePanes/wifidevicepane.ui
    statusCenter/devicePanes/wireddevicepane.ui
    statusCenter/eap/securityeappeap.ui
    statusCenter/eap/securityeapunsupported.ui
    statusCenter/networkstatuscenterleftpane.ui
    statusCenter/networkstatuscenterpane.ui
    statusCenter/popovers/connectionselectionpopover.ui
    statusCenter/popovers/simsettingspopover.ui
    statusCenter/popovers/tetheringsettingspopover.ui
    statusCenter/popovers/unlockmodempopover.ui
    statusCenter/popovers/wirelessnetworkselectionpopover.ui
)

set(HEADERS
    chunk/chunkupdater.h
    chunk/genericchunkupdater.h
    chunk/networkchunk.h
    chunk/wiredchunkupdater.h
    chunk/wirelesschunkupdater.h
    networkplugincommon.h
    models/deviceconnectionlistmodel.h
    models/wirelessaccesspointsmodel.h
    models/wirelessconnectionlistmodel.h
    models/wirelessnetworklistdelegate.h
    onboarding/onboardingnetwork.h
    onboarding/wiredonboardingsetup.h
    onboarding/wirelessonboardingsetup.h
    plugin.h
    statusCenter/connectionEditor/connectioneditorpane.h
    statusCenter/connectionEditor/ipv4connectioneditorpane.h
    statusCenter/connectionEditor/networkconnectioneditor.h
    statusCenter/connectionEditor/wificonnectioneditorpane.h
    statusCenter/devicePanes/abstractdevicepane.h
    statusCenter/devicePanes/cellularpane.h
    statusCenter/devicePanes/devicepane.h
    statusCenter/devicePanes/wifidevicepane.h
    statusCenter/devicePanes/wireddevicepane.h
    statusCenter/eap/securityeap.h
    statusCenter/eap/securityeappeap.h
    statusCenter/eap/securityeapunsupported.h
    statusCenter/networkstatuscenterleftpane.h
    statusCenter/networkstatuscenterpane.h
    statusCenter/popovers/connectionselectionpopover.h
    statusCenter/popovers/simsettingspopover.h
    statusCenter/popovers/tetheringsettingspopover.h
    statusCenter/popovers/unlockmodempopover.h
    statusCenter/popovers/wirelessnetworkselectionpopover.h
    switchmanager.h
)

set(PLUGIN_NAME network)
add_plugin_td(${PLUGIN_NAME})
target_sources(plugin-${PLUGIN_NAME} PRIVATE ${SOURCES} ${HEADERS})

target_link_libraries(plugin-${PLUGIN_NAME} Qt6::Widgets Qt6::Multimedia KF5::NetworkManagerQt KF5::ModemManagerQt PkgConfig::qrencode libcontemporary libthedesk)

cmake_minimum_required(VERSION 3.4.0)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(libcontemporary)
find_package(KF${KF_VERSION_MAJOR}BluezQt)

set(SOURCES
    btagent.cpp
    btobex.cpp
    chunk/bluetoothchunk.cpp
    bluetoothplugincommon.cpp
    devicedelegate.cpp
    plugin.cpp
    settings/bluetoothsettingspane.cpp
    settings/devicepopover.cpp
    settings/pairpopover.cpp

    settings/bluetoothsettingspane.ui
    settings/devicepopover.ui
    settings/pairpopover.ui
)

set(HEADERS
    btagent.h
    btobex.h
    chunk/bluetoothchunk.h
    bluetoothplugincommon.h
    devicedelegate.h
    plugin.h
    settings/bluetoothsettingspane.h
    settings/devicepopover.h
    settings/pairpopover.h
)

set(PLUGIN_NAME bluetooth)
add_plugin_td(${PLUGIN_NAME})
target_sources(plugin-${PLUGIN_NAME} PRIVATE ${SOURCES} ${HEADERS})
cntp_defaults_file(${PLUGIN_NAME}
    DEFAULTS_FILE thedesk-bluetooth.conf)

target_link_libraries(plugin-${PLUGIN_NAME} Qt::Widgets KF${KF_VERSION_MAJOR}::BluezQt libcontemporary libthedesk)

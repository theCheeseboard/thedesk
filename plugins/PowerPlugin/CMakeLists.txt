cmake_minimum_required(VERSION 3.4.0)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(libcontemporary)

set(SOURCES
    eventhandler.cpp
    plugin.cpp
    settings/powersettings.cpp
    sleepmonitor.cpp

    settings/powersettings.ui
)

set(HEADERS
    eventhandler.h
    plugin.h
    settings/powersettings.h
    sleepmonitor.h
)

set(PLUGIN_NAME power)
add_plugin_td(${PLUGIN_NAME})
target_sources(plugin-${PLUGIN_NAME} PRIVATE ${SOURCES} ${HEADERS})
cntp_defaults_file(${PLUGIN_NAME}
    DEFAULTS_FILE thedesk-power.conf)

target_link_libraries(plugin-${PLUGIN_NAME} Qt::Widgets libcontemporary libthedesk)

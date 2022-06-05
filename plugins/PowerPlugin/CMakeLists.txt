cmake_minimum_required(VERSION 3.4.0)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
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

target_link_libraries(plugin-${PLUGIN_NAME} Qt6::Widgets libcontemporary libthedesk)

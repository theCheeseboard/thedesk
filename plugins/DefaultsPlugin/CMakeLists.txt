cmake_minimum_required(VERSION 3.4.0)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(libcontemporary)

set(SOURCES
    defaultspane.cpp
    plugin.cpp

    defaultspane.ui
)

set(HEADERS
    defaultspane.h
    plugin.h
)

set(PLUGIN_NAME defaults)
add_plugin_td(${PLUGIN_NAME})
target_sources(plugin-${PLUGIN_NAME} PRIVATE ${SOURCES} ${HEADERS})

target_link_libraries(plugin-${PLUGIN_NAME} Qt6::Widgets libcontemporary libthedesk)

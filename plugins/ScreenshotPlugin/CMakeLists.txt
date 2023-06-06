cmake_minimum_required(VERSION 3.4.0)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Svg)
find_package(libcontemporary)

set(SOURCES
    eventhandler.cpp
    plugin.cpp
)

set(HEADERS
    eventhandler.h
    plugin.h
)

set(PLUGIN_NAME screenshot)
add_plugin_td(${PLUGIN_NAME})
target_sources(plugin-${PLUGIN_NAME} PRIVATE ${SOURCES} ${HEADERS})

target_link_libraries(plugin-${PLUGIN_NAME} Qt::Widgets Qt::Svg libcontemporary libthedesk)

cmake_minimum_required(VERSION 3.4.0)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(libcontemporary)

set(SOURCES
    plugin.cpp
    screenbrightnesschunk.cpp

    screenbrightnesschunk.ui
)

set(HEADERS
    plugin.h
    screenbrightnesschunk.h
)

set(PLUGIN_NAME brightness)
add_plugin_td(${PLUGIN_NAME})
target_sources(plugin-${PLUGIN_NAME} PRIVATE ${SOURCES} ${HEADERS})

target_link_libraries(plugin-${PLUGIN_NAME} Qt::Widgets libcontemporary libthedesk)

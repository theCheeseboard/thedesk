cmake_minimum_required(VERSION 3.4.0)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(libcontemporary)

set(SOURCES
    accessibilitydaemon.cpp
    chunks/stickykeyschunk.cpp
    plugin.cpp
    settings/accessibilitysettingspane.cpp

    settings/accessibilitysettingspane.ui
)

set(HEADERS
    accessibilitydaemon.h
    chunks/stickykeyschunk.h
    plugin.h
    settings/accessibilitysettingspane.h
)

set(PLUGIN_NAME accessibility)
add_plugin_td(${PLUGIN_NAME})
target_sources(plugin-${PLUGIN_NAME} PRIVATE ${SOURCES} ${HEADERS})

target_link_libraries(plugin-${PLUGIN_NAME} Qt::Widgets libcontemporary libthedesk)

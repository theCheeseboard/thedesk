cmake_minimum_required(VERSION 3.4.0)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
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
cntp_defaults_file(${PLUGIN_NAME}
    DEFAULTS_FILE thedesk-defaults.conf)

target_link_libraries(plugin-${PLUGIN_NAME} Qt${QT_VERSION_MAJOR}::Widgets libcontemporary libthedesk)

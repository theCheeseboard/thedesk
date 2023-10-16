cmake_minimum_required(VERSION 3.24.0)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets DBus)
find_package(libcontemporary)

set(SOURCES
    OverviewPane/overviewpane.cpp
    OverviewPane/worldclock.cpp
    clockchunk.cpp
    plugin.cpp

    OverviewPane/overviewpane.ui
    OverviewPane/worldclock.ui
    clockchunk.ui
)

set(HEADERS
    OverviewPane/overviewpane.h
    OverviewPane/worldclock.h
    clockchunk.h
    plugin.h
)

set(PLUGIN_NAME overview)
add_plugin_td(${PLUGIN_NAME})
target_sources(plugin-${PLUGIN_NAME} PRIVATE ${SOURCES} ${HEADERS})

target_link_libraries(plugin-${PLUGIN_NAME} Qt::Widgets Qt::Multimedia libcontemporary libthedesk)

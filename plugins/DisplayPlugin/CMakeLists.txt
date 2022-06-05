cmake_minimum_required(VERSION 3.4.0)

find_package(Qt6 REQUIRED COMPONENTS Widgets DBus Positioning)
find_package(libcontemporary)

set(SOURCES
    arrange/arrangecontroller.cpp
    arrange/arrangewidget.cpp
    arrange/overlaywindow.cpp
    arrange/screenarranger.cpp
    plugin.cpp
    redshift/colorramp.cpp
    redshift/redshiftdaemon.cpp
    settings/displaysettings.cpp

    arrange/arrangewidget.ui
    arrange/overlaywindow.ui
    settings/displaysettings.ui
)

set(HEADERS
    arrange/arrangecontroller.h
    arrange/arrangewidget.h
    arrange/overlaywindow.h
    arrange/screenarranger.h
    plugin.h
    redshift/colorramp.h
    redshift/redshiftdaemon.h
    settings/displaysettings.h
)

set(PLUGIN_NAME display)
add_plugin_td(${PLUGIN_NAME})
target_sources(plugin-${PLUGIN_NAME} PRIVATE ${SOURCES} ${HEADERS})

target_link_libraries(plugin-${PLUGIN_NAME} Qt6::Widgets Qt6::Multimedia Qt6::Positioning libcontemporary libthedesk)

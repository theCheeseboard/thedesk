cmake_minimum_required(VERSION 3.4.0)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Svg)
find_package(libcontemporary)

set(SOURCES
    eventhandler.cpp
    penbutton.cpp
    plugin.cpp
    screenshotwindow.cpp

    screenshotwindow.ui

    screenshot_resources.qrc
)

set(HEADERS
    eventhandler.h
    penbutton.h
    plugin.h
    screenshotwindow.h
)

set(PLUGIN_NAME screenshot)
add_plugin_td(${PLUGIN_NAME})
target_sources(plugin-${PLUGIN_NAME} PRIVATE ${SOURCES} ${HEADERS})

target_link_libraries(plugin-${PLUGIN_NAME} Qt::Widgets Qt::Svg libcontemporary libthedesk)

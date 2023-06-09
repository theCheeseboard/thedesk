cmake_minimum_required(VERSION 3.4.0)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(libcontemporary)
find_package(KF${KF_VERSION_MAJOR}PulseAudioQt REQUIRED)

set(SOURCES
    audiochunk.cpp
    audioquickwidget.cpp
    common.cpp
    eventhandler.cpp
    micchunk.cpp
    plugin.cpp
    quickwidgetsink.cpp
    quickwidgetsinkinput.cpp
)

set(HEADERS
    audiochunk.h
    audioquickwidget.h
    common.h
    eventhandler.h
    micchunk.h
    plugin.h
    quickwidgetsink.h
    quickwidgetsinkinput.h
)

set(PLUGIN_NAME pulseaudio)
add_plugin_td(${PLUGIN_NAME})
target_sources(plugin-${PLUGIN_NAME} PRIVATE ${SOURCES} ${HEADERS})
cntp_defaults_file(${PLUGIN_NAME}
    DEFAULTS_FILE thedesk-audio.conf)

target_link_libraries(plugin-${PLUGIN_NAME} Qt${QT_VERSION_MAJOR}::Widgets KF${KF_VERSION_MAJOR}::PulseAudioQt libcontemporary libthedesk)

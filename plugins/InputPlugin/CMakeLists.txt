cmake_minimum_required(VERSION 3.4.0)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(libcontemporary)
cntp_find_pkgconfig(X11 x11)
cntp_find_pkgconfig(X11_XI xi)
cntp_find_pkgconfig(X11_XORG_LIBINPUT xorg-libinput)

set(SOURCES
    backends/settingsbackend.cpp
    chunks/keyboardlayoutchunk.cpp
    daemons/keyboarddaemon.cpp
    models/selectedkeyboardlayoutsmodel.cpp
    plugin.cpp
    settings/inputsettingsleftpane.cpp
    settings/inputsettingspane.cpp
    settings/keyboardsettings.cpp
    settings/mousesettings.cpp

    chunks/keyboardlayoutchunk.ui
    settings/inputsettingsleftpane.ui
    settings/inputsettingspane.ui
    settings/keyboardsettings.ui
    settings/mousesettings.ui
)

set(HEADERS
    backends/settingsbackend.h
    chunks/keyboardlayoutchunk.h
    daemons/keyboarddaemon.h
    models/selectedkeyboardlayoutsmodel.h
    plugin.h
    settings/inputsettingsleftpane.h
    settings/inputsettingspane.h
    settings/keyboardsettings.h
    settings/mousesettings.h
)

set(PLUGIN_NAME input)
add_plugin_td(${PLUGIN_NAME})
target_sources(plugin-${PLUGIN_NAME} PRIVATE ${SOURCES} ${HEADERS})

IF(${X11_FOUND})
    target_link_libraries(plugin-${PLUGIN_NAME} PkgConfig::X11)
    target_compile_definitions(plugin-${PLUGIN_NAME} PRIVATE HAVE_X11)

    IF(${X11_XI_FOUND} AND ${X11_XORG_LIBINPUT_FOUND})
        target_link_libraries(plugin-${PLUGIN_NAME} PkgConfig::X11_XI PkgConfig::X11_XORG_LIBINPUT)
        target_compile_definitions(plugin-${PLUGIN_NAME} PRIVATE HAVE_XI)

        target_sources(plugin-${PLUGIN_NAME} PRIVATE
            backends/xinputbackend.cpp
            backends/xinputbackend.h
        )
    ENDIF()
ENDIF()

target_link_libraries(plugin-${PLUGIN_NAME} Qt::Widgets libcontemporary libthedesk)

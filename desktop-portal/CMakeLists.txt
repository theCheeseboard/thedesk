cmake_minimum_required(VERSION 3.24.0)

project(xdg-portal VERSION 1.0.0 LANGUAGES C CXX)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS WaylandClient)

find_program(WAYLAND_SCANNER wayland-scanner)
if(NOT WAYLAND_SCANNER)
    message(FATAL_ERROR "wayland-scanner not found!")
endif()
message(STATUS "Found wayland-scanner: ${WAYLAND_SCANNER}")

find_program(QT_WAYLAND_SCANNER qtwaylandscanner)
if(NOT QT_WAYLAND_SCANNER)
    message(FATAL_ERROR "qtwaylandscanner not found!")
endif()
message(STATUS "Found qtwaylandscanner: ${QT_WAYLAND_SCANNER}")

function(portal_register_wayland_protocol_extension target)
    set(multiValueArgs FILES)
    cmake_parse_arguments(REGISTER_WAYLAND_PROTOCOL_EXTENSIONS "" "" "${multiValueArgs}" ${ARGN})

    foreach(_file ${REGISTER_WAYLAND_PROTOCOL_EXTENSIONS_FILES})
        get_filename_component(_basename ${_file} NAME_WE)

        # Wayland scanner
        add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/wayland-${_basename}-client-protocol.h"
                           COMMAND ${WAYLAND_SCANNER} client-header ${CMAKE_CURRENT_SOURCE_DIR}/${_file} "${CMAKE_CURRENT_BINARY_DIR}/wayland-${_basename}-client-protocol.h"
                           DEPENDS ${_file}
                           VERBATIM)

        add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/wayland-${_basename}-client-protocol.c"
                           COMMAND ${WAYLAND_SCANNER} public-code ${CMAKE_CURRENT_SOURCE_DIR}/${_file} "${CMAKE_CURRENT_BINARY_DIR}/wayland-${_basename}-client-protocol.c"
                           DEPENDS ${_file}
                           VERBATIM)

        # Qt Wayland scanner
        add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/qwayland-${_basename}.h"
                           COMMAND ${QT_WAYLAND_SCANNER} client-header ${CMAKE_CURRENT_SOURCE_DIR}/${_file} > "${CMAKE_CURRENT_BINARY_DIR}/qwayland-${_basename}.h"
                           DEPENDS ${_file}
                           VERBATIM)

        add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/qwayland-${_basename}.cpp"
                           COMMAND ${QT_WAYLAND_SCANNER} client-code ${CMAKE_CURRENT_SOURCE_DIR}/${_file} > "${CMAKE_CURRENT_BINARY_DIR}/qwayland-${_basename}.cpp"
                           DEPENDS ${_file}
                           VERBATIM)

        # Include in sources
        set_property(SOURCE "${CMAKE_CURRENT_BINARY_DIR}/qwayland-${_basename}.cpp" PROPERTY SKIP_AUTOGEN ON)
        set_property(SOURCE "${CMAKE_CURRENT_BINARY_DIR}/qwayland-${_basename}.h" PROPERTY SKIP_AUTOGEN ON)
        set_property(SOURCE "${CMAKE_CURRENT_BINARY_DIR}/wayland-${_basename}-client-protocol.c" PROPERTY SKIP_AUTOGEN ON)
        set_property(SOURCE "${CMAKE_CURRENT_BINARY_DIR}/wayland-${_basename}-client-protocol.h" PROPERTY SKIP_AUTOGEN ON)
        target_sources(${target} PRIVATE
            "${CMAKE_CURRENT_BINARY_DIR}/wayland-${_basename}-client-protocol.c"
            "${CMAKE_CURRENT_BINARY_DIR}/wayland-${_basename}-client-protocol.h"
            "${CMAKE_CURRENT_BINARY_DIR}/qwayland-${_basename}.h"
            "${CMAKE_CURRENT_BINARY_DIR}/qwayland-${_basename}.cpp")
    endforeach()
endfunction()

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network DBus)
find_package(libcontemporary REQUIRED)
find_package(libtdesktopenvironment REQUIRED)
find_package(libthefrisbee REQUIRED)
find_package(libthefile REQUIRED)

set(SOURCES
    main.cpp
    portalhandle.cpp
    portalcommon.cpp
    screenshotmanager.cpp

    interfaces/filechooserinterface.cpp
    interfaces/settingsinterface.cpp
    interfaces/accessinterface.cpp
    interfaces/accountinterface.cpp
    interfaces/screenshotinterface.cpp
    interfaces/notificationinterface.cpp

    dialogs/filedialog.cpp dialogs/filedialog.ui
    dialogs/accountdialog.cpp dialogs/accountdialog.ui
    dialogs/screenshotwindow.cpp dialogs/screenshotwindow.ui
    dialogs/accessdialog.cpp dialogs/accessdialog.ui
    dialogs/penbutton.cpp

    backend/abstractportalbackend.cpp
    backend/x11portalbackend.cpp
    backend/waylandportalbackend.cpp
    backend/private/waylandportalscreencopyframe.h

    screenshot_resources.qrc
)

set(HEADERS
    portalhandle.h
    portalcommon.h
    screenshotmanager.h

    interfaces/filechooserinterface.h
    interfaces/settingsinterface.h
    interfaces/accessinterface.h
    interfaces/accountinterface.h
    interfaces/screenshotinterface.h
    interfaces/notificationinterface.h

    dialogs/filedialog.h
    dialogs/accountdialog.h
    dialogs/screenshotwindow.h
    dialogs/penbutton.h
    dialogs/accessdialog.h

    backend/abstractportalbackend.h
    backend/x11portalbackend.h
    backend/waylandportalbackend.h
    backend/private/waylandportalscreencopyframe.cpp
)

add_executable(xdg-portal ${SOURCES} ${HEADERS})

set_target_properties(xdg-portal PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER com.vicr123.thedesk.xdg-portal
        WIN32_EXECUTABLE TRUE
        CNTP_DATA_SUBDIR thedesk/portal)

cntp_init(xdg-portal 20)
cntp_translate(xdg-portal)
cntp_target_name(xdg-portal "thedesk-desktop-portal")
#cntp_defaults_file(xdg-portal
#    DEFAULTS_FILE xdg-portal.conf)

portal_register_wayland_protocol_extension(xdg-portal FILES
    backend/wlr-screencopy-unstable-v1.xml
)

target_link_libraries(xdg-portal Qt::Widgets Qt::Network Qt::DBus Qt::WaylandClient Qt::GuiPrivate libcontemporary libtdesktopenvironment libthedesk libthefile)
target_include_directories(xdg-portal PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS xdg-portal
        RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
        BUNDLE DESTINATION /Applications)

configure_file(org.freedesktop.impl.portal.desktop.thedesk.service.in ${CMAKE_BINARY_DIR}/org.freedesktop.impl.portal.desktop.thedesk.service)
configure_file(xdg-desktop-portal-thedesk.service.in ${CMAKE_BINARY_DIR}/xdg-desktop-portal-thedesk.service)

install(FILES thedesk.portal
    DESTINATION ${CMAKE_INSTALL_DATADIR}/xdg-desktop-portal/portals)
install(FILES thedesk-portals.conf
    DESTINATION ${CMAKE_INSTALL_DATADIR}/xdg-desktop-portal)
install(FILES ${CMAKE_BINARY_DIR}/org.freedesktop.impl.portal.desktop.thedesk.service
    DESTINATION ${CMAKE_INSTALL_DATADIR}/dbus-1/services)
install(FILES ${CMAKE_BINARY_DIR}/xdg-desktop-portal-thedesk.service
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/systemd/user/)

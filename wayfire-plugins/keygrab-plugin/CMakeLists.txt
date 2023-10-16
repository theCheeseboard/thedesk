cmake_minimum_required(VERSION 3.24.0)

project(lib VERSION 1.0.0 LANGUAGES C CXX)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(libcontemporary REQUIRED)
cntp_find_pkgconfig(wayfire wayfire)
cntp_find_pkgconfig(pixman pixman-1)

set(SOURCES
    keygrabplugin.cpp
)

set(HEADERS
    keygrabplugin.h
)

set(PRIVATE_HEADERS

)

add_library(wf-thedesk-keygrab SHARED)
cntp_init(wf-thedesk-keygrab 20)
set_target_properties(wf-thedesk-keygrab PROPERTIES
        OUTPUT_NAME thedesk-keygrab
)

find_file(TDESKTOPENVIRONMENT_KEYGRAB_FILE NAMES tdesktopenvironment-keygrab-v1.xml
    PATHS ENV XDG_DATA_DIRS
    PATH_SUFFIXES libtdesktopenvironment/wayland-protocols/tdesktopenvironment-protocols
    NO_DEFAULT_PATH
    REQUIRED
)

if(NOT TDESKTOPENVIRONMENT_KEYGRAB_FILE)
    message(FATAL_ERROR "Could not find tdesktopenvironment-keygrab-v1.xml")
endif()

thedesk_wayfire_plugin_wayland_protocols(wf-thedesk-keygrab FILES
    ${TDESKTOPENVIRONMENT_KEYGRAB_FILE}
)

target_link_libraries(wf-thedesk-keygrab Qt::Core libcontemporary PkgConfig::wayfire PkgConfig::pixman)
target_compile_definitions(wf-thedesk-keygrab PRIVATE KEYGRABPLUGIN_LIBRARY WLR_USE_UNSTABLE)

target_sources(wf-thedesk-keygrab PRIVATE ${SOURCES} ${HEADERS} ${PRIVATE_HEADERS})
install(TARGETS wf-thedesk-keygrab
    INCLUDES DESTINATION ${INCLUDE_INSTALL_DIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/wayfire
)

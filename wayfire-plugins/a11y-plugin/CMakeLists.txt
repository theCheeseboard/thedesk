cmake_minimum_required(VERSION 3.24.0)

project(lib VERSION 1.0.0 LANGUAGES C CXX)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(libcontemporary REQUIRED)
cntp_find_pkgconfig(wayfire wayfire)
cntp_find_pkgconfig(pixman pixman-1)

set(SOURCES
    a11yplugin.cpp
    abstracta11ycontroller.cpp
    stickykeys.cpp
)

set(HEADERS
    a11yplugin.h
    abstracta11ycontroller.h
    stickykeys.h
)

set(PRIVATE_HEADERS

)

add_library(wf-thedesk-a11y SHARED)
cntp_init(wf-thedesk-a11y 20)
set_target_properties(wf-thedesk-a11y PROPERTIES
        OUTPUT_NAME thedesk-a11y
)

#find_file(TDESKTOPENVIRONMENT_KEYGRAB_FILE NAMES tdesktopenvironment-keygrab-v1.xml
#    PATHS ENV XDG_DATA_DIRS
#    PATH_SUFFIXES libtdesktopenvironment/wayland-protocols/tdesktopenvironment-protocols
#    NO_DEFAULT_PATH
#    REQUIRED
#)

#if(NOT TDESKTOPENVIRONMENT_KEYGRAB_FILE)
#    message(FATAL_ERROR "Could not find tdesktopenvironment-keygrab-v1.xml")
#endif()

#thedesk_wayfire_plugin_wayland_protocols(wf-thedesk-a11y FILES
#    ${TDESKTOPENVIRONMENT_KEYGRAB_FILE}
#)

target_link_libraries(wf-thedesk-a11y Qt::Core libcontemporary PkgConfig::wayfire PkgConfig::pixman)
target_compile_definitions(wf-thedesk-a11y PRIVATE A11YPLUGIN_LIBRARY WLR_USE_UNSTABLE QT_NO_KEYWORDS)

target_sources(wf-thedesk-a11y PRIVATE ${SOURCES} ${HEADERS} ${PRIVATE_HEADERS})
install(TARGETS wf-thedesk-a11y
    INCLUDES DESTINATION ${INCLUDE_INSTALL_DIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/wayfire
)

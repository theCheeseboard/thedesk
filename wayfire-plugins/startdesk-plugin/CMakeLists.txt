cmake_minimum_required(VERSION 3.24.0)

project(lib VERSION 1.0.0 LANGUAGES C CXX)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(libcontemporary REQUIRED)
cntp_find_pkgconfig(wayfire wayfire)
cntp_find_pkgconfig(pixman pixman-1)

set(SOURCES
    startdeskplugin.cpp
)

set(HEADERS
    startdeskplugin.h
)

set(PRIVATE_HEADERS

)

add_library(wf-thedesk-startdesk SHARED)
cntp_init(wf-thedesk-startdesk 20)
set_target_properties(wf-thedesk-startdesk PROPERTIES
        OUTPUT_NAME thedesk-startdesk
)
target_link_libraries(wf-thedesk-startdesk Qt::Core libcontemporary PkgConfig::wayfire PkgConfig::pixman)
target_compile_definitions(wf-thedesk-startdesk PRIVATE STARTDESKPLUGIN_LIBRARY WLR_USE_UNSTABLE)

target_sources(wf-thedesk-startdesk PRIVATE ${SOURCES} ${HEADERS} ${PRIVATE_HEADERS})
install(TARGETS wf-thedesk-startdesk
    INCLUDES DESTINATION ${INCLUDE_INSTALL_DIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/wayfire
)

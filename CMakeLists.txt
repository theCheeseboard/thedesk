cmake_minimum_required(VERSION 3.1.0)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15)

project(thedesk VERSION 1.0.0 LANGUAGES CXX)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    link_directories(/usr/local/lib)
ENDIF()

add_subdirectory(libthedesk)
add_subdirectory(startdesk)
add_subdirectory(desktop)
add_subdirectory(platform)
add_subdirectory(plugins)

# TODO: Port when KF6 is available:
# add_subdirectory(polkitagent)

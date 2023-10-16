cmake_minimum_required(VERSION 3.24.0)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15)

project(thedesk VERSION 1.0.0 LANGUAGES CXX)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    link_directories(/usr/local/lib)
ENDIF()

find_package(QT REQUIRED NAMES Qt6)
set(KF_VERSION_MAJOR ${QT_VERSION_MAJOR} CACHE STRING "Major version of KDE Frameworks to link to")

add_subdirectory(libthedesk)
add_subdirectory(startdesk)
add_subdirectory(desktop)
add_subdirectory(platform)
add_subdirectory(plugins)
add_subdirectory(polkitagent)
add_subdirectory(locker)
add_subdirectory(desktop-portal)
add_subdirectory(wayfire-plugins)

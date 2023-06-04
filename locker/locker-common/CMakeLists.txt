project(locker VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(libcontemporary REQUIRED)
cntp_find_pkgconfig(X11 x11)

set(SOURCES
    lockergrabs.cpp
    )

set(HEADERS
    lockergrabs.h
    )

add_library(locker-common STATIC ${SOURCES} ${HEADERS})
target_link_libraries(locker-common Qt::Core libcontemporary)

IF(${X11_FOUND})
    target_link_libraries(locker-common PkgConfig::X11)
    target_compile_definitions(locker-common PRIVATE HAVE_X11)
ENDIF()

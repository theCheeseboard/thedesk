project(locker VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets DBus Sql PrintSupport)
find_package(libcontemporary REQUIRED)
find_package(libtdesktopenvironment REQUIRED)
cntp_find_pkgconfig(X11 x11)

set(SOURCES
        main.cpp
        mainwindow.cpp mainwindow.ui
        underlineanimation.cpp
        lockmanager.cpp
    )

set(HEADERS
        mainwindow.h
        underlineanimation.h
        lockmanager.h
    )

add_executable(locker-ui ${SOURCES} ${HEADERS})

set_target_properties(locker-ui PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
        MACOSX_BUNDLE_GUI_IDENTIFIER com.vicr123.locker
        WIN32_EXECUTABLE TRUE
        CNTP_DATA_SUBDIR locker)

cntp_init(locker-ui 20)
cntp_translate(locker-ui)
cntp_target_name(locker-ui "td-locker")

target_link_libraries(locker-ui Qt::Widgets Qt::DBus Qt::Sql Qt::PrintSupport libcontemporary libtdesktopenvironment libthedesk locker-common)
target_include_directories(locker-ui PUBLIC ../locker-common/)

install(TARGETS locker-ui
        RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
        BUNDLE DESTINATION /Applications)

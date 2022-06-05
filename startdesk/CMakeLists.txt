cmake_minimum_required(VERSION 3.21.0)

project(startdesk VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network DBus)
find_package(libcontemporary REQUIRED)
find_package(libtdesktopenvironment REQUIRED)

set(SOURCES
    crash/backtracedialog.cpp
    crash/backtracepopover.cpp
    crash/backtracewidget.cpp
    crash/crashwidget.cpp
    main.cpp
    splash/splashcontroller.cpp
    splash/splashwidget.cpp
    splashwindow.cpp

    crash/backtracedialog.ui
    crash/backtracepopover.ui
    crash/backtracewidget.ui
    crash/crashwidget.ui
    splash/splashwidget.ui
    splashwindow.ui

    resources.qrc
)

set(HEADERS
    crash/backtracedialog.h
    crash/backtracepopover.h
    crash/backtracewidget.h
    crash/crashwidget.h
    splash/splashcontroller.h
    splash/splashwidget.h
    splashwindow.h
)

add_executable(startdesk ${SOURCES} ${HEADERS})

set_target_properties(startdesk PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER com.vicr123.thedesk.startdesk
        WIN32_EXECUTABLE TRUE
        CNTP_DATA_SUBDIR startdesk)

cntp_init(startdesk 17)
cntp_translate(startdesk)
cntp_target_name(startdesk "startdesk")
#cntp_defaults_file(startdesk
#    DEFAULTS_FILE startdesk.conf)

target_link_libraries(startdesk Qt6::Widgets Qt6::Network Qt6::DBus libcontemporary libtdesktopenvironment libthedesk)
target_include_directories(startdesk PUBLIC ../libstartdesk/)

install(TARGETS startdesk
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE DESTINATION /Applications)

    # TODO: Install theDesk desktop file

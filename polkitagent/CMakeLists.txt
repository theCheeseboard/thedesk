cmake_minimum_required(VERSION 3.21.0)

project(polkitagent VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network DBus)
find_package(libcontemporary REQUIRED)
find_package(libtdesktopenvironment REQUIRED)
find_package(PolkitQt6-1 REQUIRED)

set(SOURCES
    authwindow.cpp
    common.cpp
    identitypopover.cpp
    main.cpp
    overlaywindow.cpp
    polkitinterface.cpp

    authwindow.ui
    identitypopover.ui
    overlaywindow.ui
)

set(HEADERS
    authwindow.h
    common.h
    identitypopover.h
    overlaywindow.h
    polkitinterface.h
)

add_executable(polkitagent ${SOURCES} ${HEADERS})

set_target_properties(polkitagent PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER com.vicr123.thedesk.polkitagent
        WIN32_EXECUTABLE TRUE
        CNTP_DATA_SUBDIR thedesk/td-polkitagent)

cntp_init(polkitagent 17)
cntp_translate(polkitagent)
cntp_target_name(polkitagent "polkitagent")
#cntp_defaults_file(polkitagent
#    DEFAULTS_FILE polkitagent.conf)

target_link_libraries(polkitagent Qt6::Widgets Qt6::Network Qt6::DBus PolkitQt6-1::Core PolkitQt6-1::Agent libcontemporary libtdesktopenvironment)

install(TARGETS polkitagent
        RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
        BUNDLE DESTINATION /Applications)

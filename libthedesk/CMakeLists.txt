cmake_minimum_required(VERSION 3.4.0)

project(lib VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia DBus Svg QuickWidgets MultimediaWidgets)
find_package(libcontemporary REQUIRED)
find_package(libtdesktopenvironment REQUIRED)

# TODO: Port NetworkManager portions
# TODO: Port Wayland portions
# TODO: Port pulseaudip portions

set(SOURCES
    actionquickwidget.cpp
    barmanager.cpp
    chunk.cpp
    common.cpp
    gatewaymanager.cpp
    gatewaysearchprovider.cpp
    hudmanager.cpp
    icontextchunk.cpp
    keygrab.cpp
    localemanager.cpp
    onboarding/onboarding.cpp
    onboarding/onboardingbar.cpp
    onboarding/onboardingcontroller.cpp
    onboarding/onboardingfinal.cpp
    onboarding/onboardingstepper.cpp
    onboarding/onboardingvideo.cpp
    onboarding/onboardingwelcome.cpp
    onboardingmanager.cpp
    onboardingpage.cpp
    plugins/pluginmanager.cpp
    powermanager.cpp
    private/keyboardlayoutmodel.cpp
    private/keyboardlayoutselector.cpp
    private/localeselector.cpp
    private/quickwidgetcontainer.cpp
    quickswitch.cpp
    quietmodemanager.cpp
    server/sessionserver.cpp
    statemanager.cpp
    statuscentermanager.cpp
    statuscenterpane.cpp
    transparentdialog.cpp

    actionquickwidget.ui
    icontextchunk.ui
    onboarding/onboarding.ui
    onboarding/onboardingbar.ui
    onboarding/onboardingfinal.ui
    onboarding/onboardingvideo.ui
    onboarding/onboardingwelcome.ui
    private/keyboardlayoutselector.ui
    private/localeselector.ui
    private/quickwidgetcontainer.ui
    transparentdialog.ui

    libthedesk_resources.qrc
)

set(HEADERS
    actionquickwidget.h
    barmanager.h
    chunk.h
    common.h
    gatewaymanager.h
    gatewaysearchprovider.h
    hudmanager.h
    icontextchunk.h
    keygrab.h
    libthedesk_global.h
    localemanager.h
    onboarding/onboarding.h
    onboarding/onboardingbar.h
    onboarding/onboardingcontroller.h
    onboarding/onboardingfinal.h
    onboarding/onboardingstepper.h
    onboarding/onboardingvideo.h
    onboarding/onboardingwelcome.h
    onboardingmanager.h
    onboardingpage.h
    plugins/pluginmanager.h
    plugins/plugininterface.h
    powermanager.h
    private/keyboardlayoutmodel.h
    private/keyboardlayoutselector.h
    private/localeselector.h
    private/onboardingmanager_p.h
    private/quickwidgetcontainer.h
    quickswitch.h
    quietmodemanager.h
    server/sessionserver.h
    statemanager.h
    statuscentermanager.h
    statuscenterpane.h
    transparentdialog.h
)

set(PRIVATE_HEADERS

)

add_library(libthedesk SHARED)
cntp_init(libthedesk 17)
set_target_properties(libthedesk PROPERTIES
        OUTPUT_NAME thedesk
        FRAMEWORK TRUE
        MACOSX_FRAMEWORK_IDENTIFIER com.vicr123.libthedesk
        VERSION 1.0.0)
cntp_target_public_headers(libthedesk
    DESTINATION_DIRECTORY libthedesk
    HEADERS ${HEADERS}
)

target_link_libraries(libthedesk Qt::Widgets Qt::DBus Qt::Multimedia Qt::Svg Qt::QuickWidgets Qt::MultimediaWidgets libcontemporary libtdesktopenvironment)
target_compile_definitions(libthedesk PRIVATE LIBTHEDESK_LIBRARY)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKECONFIG_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/libthedesk.framework/Resources/CMake)
    set(HEADER_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
    set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/libthedesk.framework/Headers)
    set(LIBRARY_INSTALL_DIR ../)
ELSE()
    set(CMAKECONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/libthedesk)
    set(HEADER_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/libthedesk)
    set(INCLUDE_INSTALL_DIR ${HEADER_INSTALL_DIR})
    set(LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
ENDIF()

target_sources(libthedesk PRIVATE ${SOURCES} ${HEADERS} ${PRIVATE_HEADERS})

configure_package_config_file(libthedeskConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/libthedeskConfig.cmake
        INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
        PATH_VARS HEADER_INSTALL_DIR LIBRARY_INSTALL_DIR)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libthedeskConfig.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR})
install(EXPORT libthedeskTargets
        FILE libthedeskTargets.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR})

install(TARGETS libthedesk
        EXPORT libthedeskTargets
        PUBLIC_HEADER DESTINATION ${HEADER_INSTALL_DIR}
        INCLUDES DESTINATION ${INCLUDE_INSTALL_DIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR})

cntp_mark_support_library_directory()

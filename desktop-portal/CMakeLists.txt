cmake_minimum_required(VERSION 3.21.0)

project(xdg-portal VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network DBus)
find_package(libcontemporary REQUIRED)
find_package(libtdesktopenvironment REQUIRED)
find_package(libthefrisbee REQUIRED)
find_package(libthefile REQUIRED)

set(SOURCES
    main.cpp
    portalhandle.cpp
    portalcommon.cpp
    screenshotmanager.cpp

    interfaces/filechooserinterface.cpp
    interfaces/settingsinterface.cpp
    interfaces/accessinterface.cpp
    interfaces/accountinterface.cpp
    interfaces/screenshotinterface.cpp

    dialogs/filedialog.cpp dialogs/filedialog.ui
    dialogs/accountdialog.cpp dialogs/accountdialog.ui
    dialogs/screenshotwindow.cpp dialogs/screenshotwindow.ui
    dialogs/accessdialog.cpp dialogs/accessdialog.ui
    dialogs/penbutton.cpp

    screenshot_resources.qrc
)

set(HEADERS
    portalhandle.h
    portalcommon.h
    screenshotmanager.h

    interfaces/filechooserinterface.h
    interfaces/settingsinterface.h
    interfaces/accessinterface.h
    interfaces/accountinterface.h
    interfaces/screenshotinterface.h

    dialogs/filedialog.h
    dialogs/accountdialog.h
    dialogs/screenshotwindow.h
    dialogs/penbutton.h
    dialogs/accessdialog.h
)


add_executable(xdg-portal ${SOURCES} ${HEADERS})

set_target_properties(xdg-portal PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER com.vicr123.thedesk.xdg-portal
        WIN32_EXECUTABLE TRUE
        CNTP_DATA_SUBDIR thedesk/portal)

cntp_init(xdg-portal 20)
cntp_translate(xdg-portal)
cntp_target_name(xdg-portal "thedesk-desktop-portal")
#cntp_defaults_file(xdg-portal
#    DEFAULTS_FILE xdg-portal.conf)

target_link_libraries(xdg-portal Qt::Widgets Qt::Network Qt::DBus libcontemporary libtdesktopenvironment libthedesk libthefile)

install(TARGETS xdg-portal
        RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
        BUNDLE DESTINATION /Applications)

configure_file(org.freedesktop.impl.portal.desktop.thedesk.service.in ${CMAKE_BINARY_DIR}/org.freedesktop.impl.portal.desktop.thedesk.service)
configure_file(xdg-desktop-portal-thedesk.service.in ${CMAKE_BINARY_DIR}/xdg-desktop-portal-thedesk.service)

install(FILES thedesk.portal
    DESTINATION ${CMAKE_INSTALL_DATADIR}/xdg-desktop-portal/portals)
install(FILES ${CMAKE_BINARY_DIR}/org.freedesktop.impl.portal.desktop.thedesk.service
    DESTINATION ${CMAKE_INSTALL_DATADIR}/dbus-1/services)
install(FILES ${CMAKE_BINARY_DIR}/xdg-desktop-portal-thedesk.service
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/systemd/user/)

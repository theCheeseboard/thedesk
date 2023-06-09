cmake_minimum_required(VERSION 3.21.0)

project(thedesk VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network DBus Multimedia QuickWidgets)
find_package(libcontemporary REQUIRED)
find_package(libtdesktopenvironment REQUIRED)

set(SOURCES
    background/background.cpp
    bar/barwindow.cpp
    bar/chunkcontainer.cpp
    bar/currentappwidget.cpp
    bar/currentappwidgetmenu.cpp
    bar/mainbarwidget.cpp
    bar/taskbarapplicationwidget.cpp
    bar/taskbardesktopwidget.cpp
    bar/taskbarwidget.cpp
    cli/commandline.cpp
    crash/crashhandling.cpp
    gateway/appsearchprovider.cpp
    gateway/appselectionmodel.cpp
    gateway/appselectionmodellistdelegate.cpp
    gateway/gateway.cpp
    gateway/gatewaysearchmodel.cpp
    gateway/gatewaysearchmodeldelegate.cpp
    gateway/maingatewaywidget.cpp
    gateway/searchresultswidget.cpp
    main.cpp
    run/rundialog.cpp
    session/endsession.cpp
    session/endsessionbutton.cpp
    statuscenter/leftpanedelegate.cpp
    statuscenter/statuscenter.cpp
    statuscenter/statuscenterleftpane.cpp
    statuscenter/statuscenterquickswitch.cpp
    systemsettings/about/about.cpp
    systemsettings/about/acknowledgements.cpp
    systemsettings/about/changehostnamepopover.cpp
    systemsettings/pluginmanagement/managepluginpopover.cpp
    systemsettings/pluginmanagement/pluginitemdelegate.cpp
    systemsettings/pluginmanagement/pluginmanagement.cpp
    systemsettings/pluginmanagement/pluginmodel.cpp
    systemsettings/recovery/recovery.cpp
    systemsettings/systemsettings.cpp
    systemsettings/systemsettingsleftpane.cpp

    background/background.ui
    bar/barwindow.ui
    bar/chunkcontainer.ui
    bar/currentappwidget.ui
    bar/currentappwidgetmenu.ui
    bar/mainbarwidget.ui
    bar/taskbardesktopwidget.ui
    bar/taskbarwidget.ui
    gateway/gateway.ui
    gateway/maingatewaywidget.ui
    gateway/searchresultswidget.ui
    run/rundialog.ui
    session/endsession.ui
    statuscenter/statuscenter.ui
    statuscenter/statuscenterleftpane.ui
    statuscenter/statuscenterquickswitch.ui
    systemsettings/about/about.ui
    systemsettings/about/acknowledgements.ui
    systemsettings/about/changehostnamepopover.ui
    systemsettings/pluginmanagement/managepluginpopover.ui
    systemsettings/pluginmanagement/pluginmanagement.ui
    systemsettings/recovery/recovery.ui
    systemsettings/systemsettings.ui
    systemsettings/systemsettingsleftpane.ui

    resources.qrc
)

set(HEADERS
    background/background.h
    bar/barwindow.h
    bar/chunkcontainer.h
    bar/currentappwidget.h
    bar/currentappwidgetmenu.h
    bar/mainbarwidget.h
    bar/taskbarapplicationwidget.h
    bar/taskbardesktopwidget.h
    bar/taskbarwidget.h
    cli/commandline.h
    crash/crashhandling.h
    gateway/appsearchprovider.h
    gateway/appselectionmodel.h
    gateway/appselectionmodellistdelegate.h
    gateway/gateway.h
    gateway/gatewaysearchmodel.h
    gateway/gatewaysearchmodeldelegate.h
    gateway/maingatewaywidget.h
    gateway/searchresultswidget.h
    run/rundialog.h
    session/endsession.h
    session/endsessionbutton.h
    statuscenter/leftpanedelegate.h
    statuscenter/statuscenter.h
    statuscenter/statuscenterleftpane.h
    statuscenter/statuscenterquickswitch.h
    systemsettings/about/about.h
    systemsettings/about/acknowledgements.h
    systemsettings/about/changehostnamepopover.h
    systemsettings/pluginmanagement/managepluginpopover.h
    systemsettings/pluginmanagement/pluginitemdelegate.h
    systemsettings/pluginmanagement/pluginmanagement.h
    systemsettings/pluginmanagement/pluginmodel.h
    systemsettings/recovery/recovery.h
    systemsettings/systemsettings.h
    systemsettings/systemsettingsleftpane.h
)

add_executable(thedesk ${SOURCES} ${HEADERS})

set_target_properties(thedesk PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER com.vicr123.thedesk
        WIN32_EXECUTABLE TRUE
        CNTP_DATA_SUBDIR thedesk)

cntp_init(thedesk 20)
cntp_translate(thedesk)
cntp_target_name(thedesk "thedesk")
cntp_defaults_file(thedesk
    DEFAULTS_FILE thedesk.conf)

target_link_libraries(thedesk Qt::Widgets Qt::Network Qt::DBus Qt::Multimedia Qt::QuickWidgets libcontemporary libtdesktopenvironment libthedesk)
target_include_directories(thedesk PUBLIC ../libthedesk/)

install(TARGETS thedesk
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE DESTINATION /Applications)

    # TODO: Port install rules over from qmake

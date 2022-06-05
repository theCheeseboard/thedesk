cmake_minimum_required(VERSION 3.4.0)

find_package(Qt6 REQUIRED COMPONENTS Widgets DBus)
find_package(libcontemporary)

set(SOURCES
    chunks/mprischunk.cpp
    dbus/notifications_adaptor.cpp
    dbus/notificationsinterface.cpp
    drawer/hudwidget.cpp
    drawer/notificationsdrawer.cpp
    drawer/notificationsdrawerwidget.cpp
    notification.cpp
    notificationtracker.cpp
    plugin.cpp
    statuscenter/notificationappgroup.cpp
    statuscenter/notificationsstatuscenterpane.cpp
    statuscenter/notificationwidget.cpp

    chunks/mprischunk.ui
    drawer/hudwidget.ui
    drawer/notificationsdrawer.ui
    drawer/notificationsdrawerwidget.ui
    statuscenter/notificationappgroup.ui
    statuscenter/notificationsstatuscenterpane.ui
    statuscenter/notificationwidget.ui
)

set(HEADERS
    chunks/mprischunk.h
    dbus/notifications_adaptor.h
    dbus/notificationsinterface.h
    drawer/hudwidget.h
    drawer/notificationsdrawer.h
    drawer/notificationsdrawerwidget.h
    notification.h
    notificationtracker.h
    plugin.h
    statuscenter/notificationappgroup.h
    statuscenter/notificationsstatuscenterpane.h
    statuscenter/notificationwidget.h
)

set(PLUGIN_NAME notifications)
add_plugin_td(${PLUGIN_NAME})
target_sources(plugin-${PLUGIN_NAME} PRIVATE ${SOURCES} ${HEADERS})

target_link_libraries(plugin-${PLUGIN_NAME} Qt6::Widgets Qt6::Multimedia libcontemporary libthedesk)

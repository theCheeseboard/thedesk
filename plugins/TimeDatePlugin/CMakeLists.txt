cmake_minimum_required(VERSION 3.4.0)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(libcontemporary)

set(SOURCES
    onboarding/onboardingtimezone.cpp
    plugin.cpp
    popovers/settimedatepopover.cpp
    popovers/settimezonepopover.cpp
    settings/datetimepane.cpp
    timezonesmodel.cpp

    onboarding/onboardingtimezone.ui
    popovers/settimedatepopover.ui
    popovers/settimezonepopover.ui
    settings/datetimepane.ui
)

set(HEADERS
    onboarding/onboardingtimezone.h
    plugin.h
    popovers/settimedatepopover.h
    popovers/settimezonepopover.h
    settings/datetimepane.h
    timezonesmodel.h
)

set(PLUGIN_NAME timedate)
add_plugin_td(${PLUGIN_NAME})
target_sources(plugin-${PLUGIN_NAME} PRIVATE ${SOURCES} ${HEADERS})
cntp_defaults_file(${PLUGIN_NAME}
    DEFAULTS_FILE thedesk-timedate.conf)

target_link_libraries(plugin-${PLUGIN_NAME} Qt::Widgets libcontemporary libthedesk)

cmake_minimum_required(VERSION 3.4.0)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(libcontemporary)

set(SOURCES
    onboarding/onboardingtheme.cpp
    plugin.cpp
    settings/accentcolourpicker.cpp
    settings/themesettingspane.cpp

    onboarding/onboardingtheme.ui
    settings/themesettingspane.ui

    thedesk-themeplugin-resources.qrc
)

set(HEADERS
    onboarding/onboardingtheme.h
    plugin.h
    settings/accentcolourpicker.h
    settings/themesettingspane.h
)

set(PLUGIN_NAME theme)
add_plugin_td(${PLUGIN_NAME})
target_sources(plugin-${PLUGIN_NAME} PRIVATE ${SOURCES} ${HEADERS})

target_link_libraries(plugin-${PLUGIN_NAME} Qt::Widgets libcontemporary libthedesk)

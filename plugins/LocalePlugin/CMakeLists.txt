cmake_minimum_required(VERSION 3.4.0)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(libcontemporary)

set(SOURCES
    localeplugincommon.cpp
    onboarding/onboardingregion.cpp
    plugin.cpp
    settings/localesettingspane.cpp

    onboarding/onboardingregion.ui
    settings/localesettingspane.ui
)

set(HEADERS
    localeplugincommon.h
    onboarding/onboardingregion.h
    plugin.h
    settings/localesettingspane.h
)

set(PLUGIN_NAME locale)
add_plugin_td(${PLUGIN_NAME})
target_sources(plugin-${PLUGIN_NAME} PRIVATE ${SOURCES} ${HEADERS})

target_link_libraries(plugin-${PLUGIN_NAME} Qt6::Widgets libcontemporary libthedesk)

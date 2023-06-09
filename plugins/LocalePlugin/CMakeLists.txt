cmake_minimum_required(VERSION 3.4.0)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
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
cntp_defaults_file(${PLUGIN_NAME}
    DEFAULTS_FILE thedesk-locale.conf)

target_link_libraries(plugin-${PLUGIN_NAME} Qt::Widgets libcontemporary libthedesk)

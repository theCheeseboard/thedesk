macro(add_plugin_td target)
    add_library(plugin-${target} SHARED)
    target_include_directories(plugin-${target} PUBLIC ../../libthedesk/)

    cntp_init_plugin(thedesk plugin-${target} 17 plugin-${target})
    cntp_translate(plugin-${target})

    set_target_properties(plugin-${target} PROPERTIES
            OUTPUT_NAME ${target}Plugin
            FRAMEWORK FALSE)
endmacro()

add_subdirectory(OverviewPlugin)
add_subdirectory(DisplayPlugin)
add_subdirectory(ScreenshotPlugin)
add_subdirectory(NotificationsPlugin)
add_subdirectory(ThemePlugin)
add_subdirectory(PowerPlugin)
add_subdirectory(LocalePlugin)
add_subdirectory(BrightnessPlugin)
add_subdirectory(DefaultsPlugin)
add_subdirectory(InputPlugin)
add_subdirectory(TimeDatePlugin)

# TODO: Port when KF6 is available:
# add_subdirectory(AudioPlugin)
# add_subdirectory(BluetoothPlugin)
# add_subdirectory(NetworkPlugion)

# TODO: Check that PolkitQt works when KF6 is available
add_subdirectory(UsersPlugin)

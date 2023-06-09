cmake_minimum_required(VERSION 3.4.0)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(libcontemporary)

cntp_define_build_dirs()

set(SOURCES
    cursorhandler.cpp
    fontformat.cpp
    iconloaderengine.cpp
    messagedialog/messagedialog.cpp
    messagedialog/messagedialogcontroller.cpp
    messagedialog/messagedialoghelper.cpp
    paletteformat.cpp
    platformtheme.cpp
    plugin.cpp

    messagedialog/messagedialog.ui

    thedesk-platform-resources.qrc
)

set(HEADERS
    cursorhandler.h
    fontformat.h
    iconloaderengine.h
    messagedialog/messagedialog.h
    messagedialog/messagedialogcontroller.h
    messagedialog/messagedialoghelper.h
    paletteformat.h
    platformtheme.h
    plugin.h
)

add_library(thedesk-platform-plugin SHARED ${SOURCES} ${HEADERS})
target_include_directories(thedesk-platform-plugin PUBLIC ../libthedesk/)

cntp_init_plugin(thedesk thedesk-platform-plugin 20 thedesk-platform-plugin)
cntp_translate(thedesk-platform-plugin)

set_target_properties(thedesk-platform-plugin PROPERTIES
        OUTPUT_NAME thedesk-platform
        FRAMEWORK FALSE)

target_link_libraries(thedesk-platform-plugin Qt::Widgets Qt::GuiPrivate libcontemporary)
target_include_directories(thedesk-platform-plugin PRIVATE ${Qt${QT_VERSION_MAJOR}Gui_PRIVATE_INCLUDE_DIRS})

install(TARGETS thedesk-platform-plugin
        LIBRARY DESTINATION ${CNTP_INSTALL_PLUGINS}/platformthemes/
        RUNTIME DESTINATION ${CNTP_INSTALL_PLUGINS}/platformthemes/)

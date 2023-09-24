find_program(WAYLAND_SCANNER wayland-scanner)
if(NOT WAYLAND_SCANNER)
    message(FATAL_ERROR "wayland-scanner not found!")
endif()
message(STATUS "Found wayland-scanner: ${WAYLAND_SCANNER}")

function(thedesk_wayfire_plugin_wayland_protocols target)
    set(multiValueArgs FILES)
    cmake_parse_arguments(REGISTER_WAYLAND_PROTOCOL_EXTENSIONS "" "" "${multiValueArgs}" ${ARGN})

    foreach(_file ${REGISTER_WAYLAND_PROTOCOL_EXTENSIONS_FILES})
        get_filename_component(_basename ${_file} NAME_WE)

        # Wayland scanner
        add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/wayland-${_basename}-server-protocol.h"
                           COMMAND ${WAYLAND_SCANNER} server-header ${_file} "${CMAKE_CURRENT_BINARY_DIR}/wayland-${_basename}-server-protocol.h"
                           DEPENDS ${_file}
                           VERBATIM)

        add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/wayland-${_basename}-server-protocol.c"
                           COMMAND ${WAYLAND_SCANNER} public-code ${_file} "${CMAKE_CURRENT_BINARY_DIR}/wayland-${_basename}-server-protocol.c"
                           DEPENDS ${_file}
                           VERBATIM)

        # Include in sources
        set_property(SOURCE "${CMAKE_CURRENT_BINARY_DIR}/wayland-${_basename}-server-protocol.c" PROPERTY SKIP_AUTOGEN ON)
        set_property(SOURCE "${CMAKE_CURRENT_BINARY_DIR}/wayland-${_basename}-server-protocol.h" PROPERTY SKIP_AUTOGEN ON)
        target_sources(${target} PRIVATE
            "${CMAKE_CURRENT_BINARY_DIR}/wayland-${_basename}-server-protocol.c"
            "${CMAKE_CURRENT_BINARY_DIR}/wayland-${_basename}-server-protocol.h")
    endforeach()
endfunction()

add_subdirectory(keygrab-plugin)

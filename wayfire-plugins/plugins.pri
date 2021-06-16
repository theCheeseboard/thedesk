equals(THELIBS_BUILDTOOLS_PATH, "") {
    THELIBS_BUILDTOOLS_PATH = $$[QT_INSTALL_PREFIX]/share/the-libs/pri
}
include($$THELIBS_BUILDTOOLS_PATH/varset.pri)

DEFINES += WLR_USE_UNSTABLE WAYFIRE_PLUGIN
CONFIG += c++17 plugin

CONFIG += link_pkgconfig
PKGCONFIG += wayfire wlroots

wayland_scanner_headers.output = wayland-${QMAKE_FILE_BASE}-server-protocol.h
wayland_scanner_headers.commands = wayland-scanner server-header ${QMAKE_FILE_NAME} ${QMAKE_FILE_OUT}
wayland_scanner_headers.input = WAYLAND_PROTOCOL_EXTENSIONS
wayland_scanner_headers.CONFIG += target_predeps no_link

wayland_scanner_sources.output = wayland-${QMAKE_FILE_BASE}-server-protocol.c
wayland_scanner_sources.commands = wayland-scanner private-code ${QMAKE_FILE_NAME} ${QMAKE_FILE_OUT}
wayland_scanner_sources.input = WAYLAND_PROTOCOL_EXTENSIONS
wayland_scanner_sources.variable_out = SOURCES
wayland_scanner_headers.CONFIG += target_predeps no_link

QMAKE_EXTRA_COMPILERS += wayland_scanner_headers wayland_scanner_sources

target.path = $$THELIBS_INSTALL_LIB/wayfire/
INSTALLS = target

equals(THELIBS_BUILDTOOLS_PATH, "") {
    THELIBS_BUILDTOOLS_PATH = $$[QT_INSTALL_PREFIX]/share/the-libs/pri
}
include($$THELIBS_BUILDTOOLS_PATH/varset.pri)

DEFINES += WLR_USE_UNSTABLE WAYFIRE_PLUGIN
CONFIG += c++17 plugin

CONFIG += link_pkgconfig
PKGCONFIG += wayfire wlroots

target.path = $$THELIBS_INSTALL_LIB/wayfire/
INSTALLS = target

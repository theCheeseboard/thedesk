QT += gui widgets thelib thefile
SHARE_APP_NAME = thedesk/portal

TARGET = thedesk-desktop-portal

CONFIG += c++20 console
CONFIG -= app_bundle

# Include the-libs build tools
include(/usr/share/the-libs/pri/buildmaster.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        dialogs/filedialog.cpp \
        interfaces/filechooserinterface.cpp \
        interfaces/settingsinterface.cpp \
        main.cpp \
        portalhandle.cpp

DBUS_ADAPTOR_HEADERS = portalhandle.h

qtPrepareTool(QDBUSCPP2XML, qdbuscpp2xml)

cpp2xml.output = ${QMAKE_FILE_BASE}.xml
cpp2xml.commands = $$QDBUSCPP2XML -a -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
cpp2xml.input = DBUS_ADAPTOR_HEADERS
cpp2xml.CONFIG = no_link target_predeps
cpp2xml.variable_out = DBUS_ADAPTORS
QMAKE_EXTRA_COMPILERS += cpp2xml

DBUS_ADAPTORS = portalhandle.xml

unix {
    target.path = $$THELIBS_INSTALL_LIB

    portaldef.files = thedesk.portal
    portaldef.path = $$THELIBS_INSTALL_PREFIX/share/xdg-desktop-portal/portals

    dbusactivation.files = org.freedesktop.impl.portal.desktop.thedesk.service
    dbusactivation.path = $$THELIBS_INSTALL_PREFIX/share/dbus-1/services

    INSTALLS += target portaldef dbusactivation
}

DISTFILES += \
    org.freedesktop.impl.portal.desktop.thedesk.service \
    thedesk.portal

HEADERS += \
    dialogs/filedialog.h \
    interfaces/filechooserinterface.h \
    interfaces/settingsinterface.h \
    portalhandle.h

FORMS += \
    dialogs/filedialog.ui

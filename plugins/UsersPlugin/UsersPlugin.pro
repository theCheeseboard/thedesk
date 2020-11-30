QT += gui widgets dbus

PKGCONFIG += polkit-qt5-1
CONFIG += link_pkgconfig

LIBS += -lcrypt

TEMPLATE = lib
CONFIG += plugin

CONFIG += c++11

# Include the-libs build tools
include(/usr/share/the-libs/pri/gentranslations.pri)

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    onboarding/onboardingusers.cpp \
    plugin.cpp \
    settings/adduserdialog.cpp \
    settings/changepassworddialog.cpp \
    settings/changerealnamedialog.cpp \
    settings/deleteuserdialog.cpp \
    settings/lockuserdialog.cpp \
    settings/user.cpp \
    settings/usersmodel.cpp \
    settings/userspane.cpp \
    settings/usertypedialog.cpp

HEADERS += \
    onboarding/onboardingusers.h \
    plugin.h \
    settings/adduserdialog.h \
    settings/changepassworddialog.h \
    settings/changerealnamedialog.h \
    settings/deleteuserdialog.h \
    settings/lockuserdialog.h \
    settings/user.h \
    settings/usersmodel.h \
    settings/userspane.h \
    settings/usertypedialog.h

DISTFILES += \
    Plugin.json \
    defaults.conf

unix {
    translations.files = translations/*.qm
    translations.path = /usr/share/thedesk/UsersPlugin/translations

    defaults.files = defaults.conf
    defaults.path = /etc/theSuite/theDesk/UsersPlugin/

    INSTALLS += translations defaults
}

include(../plugins.pri)

FORMS += \
    onboarding/onboardingusers.ui \
    settings/adduserdialog.ui \
    settings/changepassworddialog.ui \
    settings/changerealnamedialog.ui \
    settings/deleteuserdialog.ui \
    settings/lockuserdialog.ui \
    settings/userspane.ui \
    settings/usertypedialog.ui
